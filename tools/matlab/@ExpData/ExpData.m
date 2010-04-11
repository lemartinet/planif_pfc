classdef ExpData < handle
    properties
        num_exp
        step_size
        path
        trial
        day
        pv_space
        v_space
        
        nbneurons
        neuron
        col_of_minicol
        
        lin_graph
        
        spatial_10
        spatial_0
        spatial_1
        spatial_2
    end
    
    methods
        function [exp_data] = ExpData(path, num, reload, maze)
            if reload
                load(sprintf('%s/%d/exp_data.mat', path, num), 'exp_data');
            else
                exp_data.num_exp = num;
                exp_data.path = sprintf('%s/%d/', path, num);
                exp_data.load_trials(maze);

                exp_data.analyze_behavior();
                exp_data.analyze_exploration();
%                 exp_data.load_trials_shortcut(maze);
%                 exp_data.analyze_behavior_shortcut();

                exp_data.load_neurons();
                save(sprintf('%s/exp_data.mat', exp_data.path), 'exp_data');
            end
        end
        
        % calcule une liste d'indexes d'enregistrement
        [idx] = get_step_idx(exp_data, days, trials)
        % charge les infos des essais
        load_trials(exp_data, maze)
        % charge les infos de comportement
        analyze_behavior(exp_data)
        % charge les infos de decision (planning VS explo)
        analyze_exploration(exp_data)
        % charge les infos des neurones
        load_neurons(exp_data)
        % crée les fichiers Q des fr selon jour/essai (en smoothant éventuellement)
        make_q(exp_data, smoothy)
        % calcule la représentation linéaire de tous les fr
        linear_graph(exp_data)
        % affiche cette représentation selon une liste de neurones, et un chemin
        [fr] = linear_graph_plot(exp_data, path, neurons)
        
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
        
        % recupere le centre de champ recepteur pour chaque neurone
        function [y] = get_centers(exp_data, list)
            y = cell2mat({exp_data.neuron(list).center}');
            % y = reshape([exp_data.neuron(list).center], 2, length(list))';
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
        
        function [m_all, m_fst] = get_mean_behavior(exp_data, days)
            m_all = hist([exp_data.trial(days, :).all_choices], 1:3);
            m_fst = hist([exp_data.trial(days, :).fst_choice], 1:3);
        end
        
        function [nb_errors] = get_nb_errors(exp_data, days, good)
            f = cellfun(@(x) [x, good], {exp_data.trial(days, :).all_choices}, 'UniformOutput', false);
            f = cellfun(@(x) find(x == good, 1) - 1, f);
            nb_errors = sum(f);
        end
        
        function [explo_ratio] = get_explo_ratio(exp_data, days)
            dec_list = [exp_data.trial(days, :).decision];
            dec_list = dec_list(2, :);
            explo_ratio = length(find(dec_list == 0)) / ...
                (length(find(dec_list == 0)) + length(find(dec_list == 1)));
        end
        
        function [day, trial] = from_timestep(exp_data, ts)
            num = find([exp_data.trial(:,:).start] - ts > 0, 1) - 1;
            [day, trial] = exp_data.from_num_trial(num);
        end
        
        function [m, s] = spatial_mean_std(exp_data, v)
            idx = exp_data.pv_space > 0;
            m = dotprod(v, exp_data.pv_space(idx));
            s = sqrt(dotprod(v .^ 2, exp_data.pv_space(idx)) - m ^ 2);
        end
    end
    
    methods (Static = true)
        function [z] = from_day_trial(x,y) 
            z = (x >= 16) * (175 + y) + (x < 16) * ((x-1) * 12 + y);
        end
        function [day, trial] = from_numtrial(x) 
            y = (x >= 176) * ([16, x - 175]) + (x < 176) * (1 + [floor((x-1)/12) , mod(x-1,12)]);
            day = y(1); trial = y(2);
        end
    end
    
end

