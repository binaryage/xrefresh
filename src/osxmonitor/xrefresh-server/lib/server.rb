require 'gserver'
require 'json'

module XRefreshServer

    # server
    class Server < GServer
        attr :clients

        def initialize(*args)
            super(*args)
            @clients = Set.new
            @last_client_id = 0
        end

        def serve(socket)
            socket.binmode
            @last_client_id += 1
            client = Client.new(@last_client_id, socket)
            @clients.add(client)
            buffer = ""
            loop do
                # accumulate incomming input in @buffer
                begin
                    buffer += socket.gets
                rescue
                    break
                end

                begin
                    # try to parse buffer
                    msg = JSON.parse buffer
                rescue
                    # buffer may be incomplete due to packet fragmentation ...
                else
                    # got whole message => process it
                    buffer = ""
                    process(client, msg)
                end
            end
        end

        def process(client, msg)
            # see windows implementation in src/winmonitor/Server.cs#ProcessMessage
            case msg["command"]
            when "Hello"
                type = msg["type"] || '?'
                agent = msg["agent"] || '?'
                $out.puts "Client ##{client.id} connected:  #{type} (#{agent})"
                client.send_about($VERSION, $AGENT)
            when "Bye"
                @clients.delete(client)
                $out.puts "Client ##{client.id} disconnected"
            when "SetPage"
                url = msg["url"] || '?'
                $out.puts "Client ##{client.id} changed page to #{url}"
            end
        end
    end
    
end