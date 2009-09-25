classdef ExpData < handle
    properties
        path
        trial
        day
        pv_space
        v_space
        
        nbneurons
        neuron
        col_of_minicol
        
        lin_graph
    end
    
    methods
        function [exp_data] = ExpData(path)
            exp_data.path = path;
        end
        
        % charge les infos des essais
        load_trials(exp_data)
        % charge les infos des neurones
        load_neurons(exp_data)
        % crée les fichiers Q des fr selon jour/essai (en smoothant éventuellement)
        make_q(exp_data, smoothy)
        % calcule la représentation linéaire de tous les fr
        linear_graph(exp_data)
        % affiche cette représentation selon une liste de neurones, et un chemin
        linear_graph_plot(exp_data, path, neurons)
        
        % charge le fr d'un neurone
        function [fr] = load_neuron(exp_data, nb)
            fr = load(sprintf('%s/frs/%d.mat', exp_data.path, nb));
            fr = fr.fr;
        end
        
        % retourne les indices de neurone en fonction de leur type x
        function [y] = from_type(exp_data, x)
            y = find([exp_data.neuron.type] == x);
        end
        
        % retourne les indices de neurone appartenant à une colonne x
        % !!! some neurons don't belong to a col (e.g., sup neuron) -> cellfun
        function [y] = from_col(exp_data, x) 
            y = find(cellfun(@(y) ~isempty(y) && y == x, {exp_data.neuron.col}) == 1);
        end
        
        % retourne les indices de neurone appartenant à une minicolonne x
        % !!! some neurons don't belong to a minicol (e.g., state neuron) -> cellfun
        function [y] = from_minicol(exp_data, x)
            y = find(cellfun(@(y) ~isempty(y) && y == x, {exp_data.neuron.minicol}) == 1);
        end
        
        % retourne les indices de minicolonnes appartenant à une colonne x
        function [y] = minicols_of_col(exp_data, x) 
            y = find(exp_data.col_of_minicol == x);
        end
        
        % retourne les indices de neurone dont le centre de ch recep est
        % proche (< z) d'une position (x,y)
        function [id] = at_position(exp_data, x, y, z) 
            id = find(cellfun(@(v) sum((v - [x y]).^2, 2), {exp_data.neuron.center}) < z);
        end
        
        % charge Q ou Qs pour un jour/essai donné
        function [y] = getQ(exp_data, x, y) 
            y = load(sprintf('%s/frs/Q_%d_%d.mat', exp_data.path, x, y));
            y = y.Q;
        end
        function [y] = getQs(exp_data, x, y) 
            y = load(sprintf('%s/frs/Qs_%d_%d.mat', exp_data.path, x, y));
            y = y.Q;
        end
        
        function [y] = get_centers(exp_data)
            y = cell2mat({exp_data.neuron.center}');
        end
        
        function plot_theta(exp_data, list)
            if ~exist('list','var')
                list = 1 : exp_data.nbneurons;
            end
            t = [exp_data.neuron(list).theta];
            t = cell2mat({t.bin}');
            figure, plot(t');
            figure, plot(mean(t));
        end
    end
    
    methods (Static = true)
        function [z] = from_dt(x,y) 
            z = (x >= 16) * (175 + y) + (x < 16) * ((x-1) * 12 + y);
        end
        function [y] = from_num(x) 
            y = (x >= 176) * ([16, x - 175]) + (x < 176) * (1 + [floor((x-1)/12) , mod(x-1,12)]);
        end
    end
    
end

