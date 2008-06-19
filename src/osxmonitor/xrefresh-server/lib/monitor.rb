require 'osx/foundation'
OSX.require_framework '/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework'
include OSX

module XRefreshServer
    
    class Monitor
        
        def initialize(server, config)
            @config = config
            @server = server
            @modified_dirs = Set.new
            @paths_info = Hash.new
            @streams = []
        end
        
        def schedule(start_event_id)
            fsevents_cb = proc do |stream, ctx, numEvents, paths, marks, eventIDs|
                # ctx doesn't work through rubycocoa?
                root = FSEventStreamCopyPathsBeingWatched(stream).first
                paths.regard_as('*')
                numEvents.times do |n|
                    $out.puts "Event: #{paths[n]}" if @config["debug"]
                    @modified_dirs.add({:root=>root, :dir=>paths[n]})
                end
            end

            @config["paths"].each do |path|
                $out.puts "  monitoring #{path}"
                # need to create new stream for every supplied path
                # because we want to report registered sources of the changes
                stream = FSEventStreamCreate(
                    KCFAllocatorDefault,
                    fsevents_cb,
                    nil,
                    [path],
                    start_event_id,
                    @config["defer_time"],
                    KFSEventStreamCreateFlagNone) #KFSEventStreamCreateFlagNoDefer
                die "Failed to create the FSEventStream" unless stream

                FSEventStreamScheduleWithRunLoop(
                    stream,
                    CFRunLoopGetCurrent(),
                    KCFRunLoopDefaultMode)

                ok = FSEventStreamStart(stream)
                die "Failed to start the FSEventStream" unless ok

                @streams << stream
            end
        end
        
        # blocking call
        def run_loop(start_time)
            
            activities = {"changed" => '*', "deleted" => '-', "created" => '+', "renamed" => '>'}
            
            # main loop
            $out.puts "Waiting for file system events ..."
            not_first_time = false
            loop do
                if @server.stopped?
                    $out.puts "Server stopped"
                    break
                end
                @streams.each do |stream|
                    FSEventStreamFlushSync(stream)
                end
                buckets = Hash.new()
                if not_first_time # all root folders are reported during first stream flush
                    @modified_dirs.each do |dir_info|
                        begin
                            dir = dir_info[:dir]
                            root = dir_info[:root]
                            unless dir=~@config["dir_include"]
                                $out.puts "debug: #{dir} rejected because dir_include" if @config["debug"]
                                next
                            end
                            if dir=~@config["dir_exclude"]
                                $out.puts "debug: #{dir} rejected because dir_exclude" if @config["debug"]
                                next
                            end

                            if File.exists?(dir)
                                $out.puts "debug: checking dir #{dir}" if @config["debug"]
                                Dir.foreach(dir) do |file|
                                    unless file=~@config["file_include"]
                                        $out.puts "debug: #{file} rejected because file_include" if @config["debug"]
                                        next
                                    end
                                    if file=~@config["file_exclude"]
                                        $out.puts "debug: #{file} rejected because file_exclude" if @config["debug"]
                                        next
                                    end

                                    full_path = File.join(dir, file)
                                    next if File.directory?(full_path)
                                    begin
                                        stat = File.stat(full_path)
                                        $out.puts "debug: stat #{full_path}" if @config["debug"]
                                    rescue
                                        # file may not exist
                                        $out.puts "debug: stat failed #{full_path}" if @config["debug"]
                                        next # keep silence
                                    end
                                    current_time = stat.mtime.to_i
                                    original_time = @paths_info[full_path] || start_time

                                    if (current_time > original_time)
                                        $out.puts "debug: reported #{full_path}" if @config["debug"]
                                        relative_path = full_path[root.size+1..-1]
                                        buckets[root]||=[]
                                        buckets[root]<< {
                                            "action" => "changed",
                                            "path1" => relative_path,
                                            "path2" => nil
                                        }
                                    end
                                    @paths_info[full_path] = current_time
                                end
                            else
                                relative_path = dir[root.size+1..-1]
                                buckets[root]||=[]
                                buckets[root]<< {
                                    "action" => "deleted",
                                    "path1" => relative_path,
                                    "path2" => nil
                                }
                            end
                        rescue
                            $out.puts "debug: exception! #{dir}" if @config["debug"]
                            raise if @config["debug"]
                            next #keep silence
                        end
                    end
                else
                    not_first_time = true
                end

                if buckets.size
                    buckets.each do |root, files|
                        $out.puts "  activity in #{root}:"
                        files.each do |file|
                            $out.puts "    #{activities[file["action"]]} #{file["path1"]}"
                        end
                        date = nil
                        time = nil
                        name = root
                        type = 'type?'

                        @server.clients.each do |client|
                            client.send_do_refresh(root, name, type, date, time, files)
                        end
                    end
                    buckets.clear
                end

                @modified_dirs.clear
                sleep @config["sleep_time"]
            end

            streams.each do |stream|
                FSEventStreamStop(stream)
                FSEventStreamInvalidate(stream)
                FSEventStreamRelease(stream)
            end
        end
    end
end


