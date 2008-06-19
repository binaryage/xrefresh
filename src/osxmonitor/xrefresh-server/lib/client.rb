require 'json'

module XRefreshServer

    # client representation on server side
    class Client
        attr :id, :dead

        def initialize(id, socket)
            @id = id
            @socket = socket
            @dead = false
        end
    
        def send(data)
            return if @dead
            begin
                @socket << data
            rescue
                $out.puts "Client ##{@id} is dead"
                @dead = true
            end
        end

        def send_about(version, agent)
            send({"command" => "AboutMe", "version" => version, "agent" => agent}.to_json)
        end

        def send_do_refresh(root, name, type, date, time, files)
            send({"command" => "DoRefresh", "root" => root, "name" => name, "date" => date, "time" => time, "type" => type, "files" => files}.to_json)
        end
    end

end