classdef myQueue < handle           
    properties
        data = {}
    end    
    methods
        function q = Queue()
            % TODO: do something in constructor if desired
        end    

        function item = dequeue(q)
            if( ~isempty(q.data))
                item = q.data(1);
                q.data = q.data(2:end);
            else
               ME = MException('Queue:empty', 'Queue is empty');
               throw(ME)
            end
        end

        function enqueue(q,item)
            q.data{end+1} = item;
        end

        function is_empty = isEmpty(q)
            is_empty = isempty(q.data);
        end
        
		function count = get_count(q)
            count = length(q.data);
        end
    end       
end


