std::vector<Eigen::Vector2d> ThickCurb(const std::vector<std::vector<Eigen::Vector2d>> curb_gt,const int pixel,const double filter_size) {

       double curb_min_x = std::numeric_limits<double>::max();
       double curb_min_y = std::numeric_limits<double>::max();
       double curb_max_x = std::numeric_limits<double>::lowest();
       double curb_max_y = std::numeric_limits<double>::lowest();
       for (const auto &c : curb_gt) {
              for (const auto &v : c) {
                    curb_min_x = std::min(curb_min_x, v(0));
           curb_min_y = std::min(curb_min_y, v(1));
           curb_max_x = std::max(curb_max_x, v(0));
           curb_max_y = std::max(curb_max_y, v(1));
                }
            }

       curb_min_x-=pixel*filter_size;
       curb_min_y-=pixel*filter_size;
       curb_max_x+=pixel*filter_size;
       curb_max_y+=pixel*filter_size;

       int total_x=(curb_max_x-curb_min_x)/filter_size+1;
       int total_y=(curb_max_y-curb_min_y)/filter_size+1;

       std::vector<std::vector<int>> vis(total_x, std::vector<int>(total_y, 0));

        const std::vector<double> dx({0, 0,-filter_size, filter_size, filter_size, filter_size,-filter_size,-filter_size});
        const std::vector<double> dy({filter_size,-filter_size, 0, 0,-filter_size, filter_size,-filter_size, filter_size});
        std::vector<Eigen::Vector2d> thick_curb;
        //std::map<std::pair<double,double>,bool> vis;
        //vis.clear();
        std::queue<std::pair<Eigen::Vector2d,int>> Q;
        for (const auto &c : curb_gt) {
            for (const auto &v : c) {
                Q.push(std::make_pair(v,0));
                vis[(v(0)-curb_min_x)/filter_size][(v(1)-curb_min_y)/filter_size]=1;
            }
        }

        while(!Q.empty()) {
            std::pair<Eigen::Vector2d,int> head=Q.front();
            Q.pop();
     thick_curb.push_back(head.first);
            if(head.second==pixel) {
       continue;
            }
            for(std::vector<double>::size_type i=0;i<dx.size();i++) {
                double nextx=head.first(0)+dx[i];
                double nexty=head.first(1)+dy[i];
                if(vis[(nextx-curb_min_x)/filter_size][(nexty-curb_min_y)/filter_size]) {
                    continue;
                }
                vis[(nextx-curb_min_x)/filter_size][(nexty-curb_min_y)/filter_size]=true;
                Q.push(std::make_pair(Eigen::Vector2d(nextx,nexty),head.second+1));
            }
        }
        return thick_curb;
    }
