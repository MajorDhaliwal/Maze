//Using SDL and standard IO
#include <stdlib.h>
#include<SDL2/SDL.h>
#include <stdio.h>
#include<iostream>
#include<unordered_map>
#include<utility>
#include<vector>
#include<stack>
#include <stdlib.h>
#include <time.h>
#include<unordered_set>
#include<queue>
#include <algorithm>  
#include<string>
#include <limits>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

std::unordered_map<std::string, SDL_Rect> grid_map; //maps cell location to address of cell attributes
std::vector<std::vector<bool>> visited (22, std::vector<bool>(30)); //bool adjency matrix to draw maze
std::vector<std::vector<bool>> visited_bfs_table (22, std::vector<bool>(30)); //bool adjency matrix to draw maze path
std::stack<std::pair<int,int>> grid_stack; //keeps track of cell coordinates in stack
std::unordered_map<std::string, bool> visted_bfs; 
std::unordered_map<std::string, std::string> pred;
int INT_MIN = std::numeric_limits<int>::min();
int INT_MAX = std::numeric_limits<int>::max();

//Declare adjanecy matrix
std::unordered_map<std::string, std::unordered_set<std::string>> adj_matrix;



 
void drawBorder(SDL_Renderer* renderer){

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 20, 20, SCREEN_WIDTH-20, 20); //top
    SDL_RenderDrawLine(renderer, 20, 20, 20, SCREEN_HEIGHT-20); //left
    SDL_RenderDrawLine(renderer, 20, SCREEN_HEIGHT-20, SCREEN_WIDTH-20, SCREEN_HEIGHT-20); //bottom
    SDL_RenderDrawLine(renderer, SCREEN_WIDTH-20, 20, SCREEN_WIDTH-20, SCREEN_HEIGHT-20)  ; //right
    
}

void drawCells(SDL_Renderer* renderer){
    //std::cout<<"drawCells function is called" << std::endl;
    for(int i=1; i<=22; i++){
        for(int j=1; j<=30; j++){
            SDL_Rect cellToDraw = {20*j,20*i,20,20};
            SDL_SetRenderDrawColor(renderer, 50, 0, 130, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &cellToDraw);
            SDL_RenderFillRect(renderer, &cellToDraw);
            std::string key = std::to_string(i-1)+"x"+std::to_string(j-1);
            grid_map[key] = cellToDraw;
            visted_bfs[key] = false;
            pred[key] = "-1";
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &cellToDraw);

        }
    }
}

void removeWall (SDL_Renderer* renderer, SDL_Rect& start, SDL_Rect& end, char dir){
    SDL_SetRenderDrawColor(renderer, 50, 0, 130, SDL_ALPHA_OPAQUE);

    if(dir=='E'){
        SDL_Rect celltoDraw = {start.x+1,start.y+1,39,19};
        SDL_RenderDrawRect(renderer, &celltoDraw);
        SDL_RenderFillRect(renderer, &celltoDraw);
    }

    else if(dir=='S'){
        SDL_Rect celltoDraw = {start.x+1,start.y+1,19,39};
        SDL_RenderDrawRect(renderer, &celltoDraw);
        SDL_RenderFillRect(renderer, &celltoDraw);
    }
    
    else if(dir=='N'){
        SDL_Rect celltoDraw = {end.x+1,end.y+1,19,39};
        SDL_RenderDrawRect(renderer, &celltoDraw);
        SDL_RenderFillRect(renderer, &celltoDraw);
    }
    
    else if(dir=='W'){
        SDL_Rect celltoDraw = {end.x+1,end.y+1,39,19};
        SDL_RenderDrawRect(renderer, &celltoDraw);
        SDL_RenderFillRect(renderer, &celltoDraw);
    }
 
}

int nextDirection(){
    int random = rand();
    int x = random%4;
    //std::cout << x << std::endl;
    return x;
}

std::vector<std::string> bfs(){
    std::queue<std::string> cell_q;
    std::vector<std::string> path;
    cell_q.push("0x0");
    visted_bfs["0x0"] = true;
    while(!cell_q.empty()){
        auto curr = cell_q.front();
        cell_q.pop();
        path.push_back(curr);
        

        for(auto nei : adj_matrix[curr]){
            if(visted_bfs[nei]==false){
                visted_bfs[nei] = true;
                pred[nei] = curr;
                cell_q.push(nei);
            }
        }

        if(curr=="21x29"){
            break;
        }

    }

    return path;

}

struct Point{
    int x;
    int y;
    char dir;
};

void dfs(SDL_Renderer* renderer, std::stack<std::pair<int,int>>& grid_stack, std::vector<std::vector<bool>>& visited){
    int count = 0;
    visited[0][0] = true;
    grid_stack.push({0,0});
    while(!grid_stack.empty()){

        auto curr = grid_stack.top();
        int i = curr.first;
        int j = curr.second;
        
        Point north, south, west, east; 
        north = (Point){i-1, j, 'N'};
        south = (Point){i+1, j, 'S'};
        west = (Point){i, j-1, 'W'};
        east = (Point){i, j+1, 'E'};

        std::vector<Point> directions = {north, south, west, east};
        auto cell = directions[nextDirection()]; //pick cell at random 
        std::pair<int,int> next_cell = {cell.x, cell.y}; //next cell
        auto cell_dir = cell.dir; // either N W S E

        

        while(next_cell.first < 0 || next_cell.first >=visited.size() || next_cell.second >=visited[0].size() || next_cell.second < 0){
                cell = directions[nextDirection()];
                next_cell = {cell.x, cell.y};
                cell_dir = cell.dir;
        }

        auto check_north = (i-1 < 0)? true : visited[north.x][north.y];
        auto check_south = (i+1 >= visited.size())? true : visited[south.x][south.y];
        auto check_west =  (j-1  < 0)? true :visited[west.x][west.y];
        auto check_east =  (j+1 >= visited[0].size())? true: visited[east.x][east.y];

        

        if(visited[next_cell.first][next_cell.second]==false){
            std::string start = std::to_string(i)+"x"+std::to_string(j);
            std::string end = std::to_string(next_cell.first)+"x"+std::to_string(next_cell.second);
            removeWall(renderer, grid_map[start], grid_map[end], cell_dir);
            SDL_Delay(10);
            SDL_RenderPresent(renderer);
            grid_stack.push(next_cell);
            /*std::cout << start;
            std::cout << "-->";
            std::cout << end+" " << cell_dir <<std::endl;*/
            adj_matrix[start].insert(end);
            adj_matrix[end].insert(start);
            visited[next_cell.first][next_cell.second]=true;
        }
        else if(check_north && check_south && check_east && check_west){
            grid_stack.pop();
        }
        else{
            while((visited[next_cell.first][next_cell.second]) && (next_cell.first < 0 || next_cell.first >=visited.size() || next_cell.second >=visited[0].size() || next_cell.second < 0)){
                cell = directions[nextDirection()];
                next_cell = {cell.x, cell.y};
                cell_dir = cell.dir;
            }
        }
       

    }

    //std::cout << "end" << std::endl;
}

std::vector<std::string> findPath(std::unordered_map<std::string, std::string> pred){
    std:: vector<std::string> path;
    std::string crawl = "21x29";
    path.push_back(crawl);
    while (pred[crawl] != "-1") {
        path.push_back(pred[crawl]);
        crawl = pred[crawl];
    }

    return path;
}

void changeCellColor(SDL_Renderer* renderer, int color, int seconds, std::string cell_position){
       auto cell = &grid_map[cell_position];
       SDL_Rect celltoDraw = {cell->x+8,cell->y+8,5,5};
       SDL_RenderDrawRect(renderer, &celltoDraw);
       SDL_RenderFillRect(renderer, &celltoDraw);
       SDL_Delay(seconds);
       SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
   srand(time(NULL)); // initiated for the nextDirection() function
   bool flag = true;

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
       
        if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;
	    
         
        drawCells(renderer);
        drawBorder(renderer);
        SDL_RenderPresent(renderer);

	    SDL_SetWindowTitle(window, "Maze Generator");
            while (!done) {
                SDL_Event event;
                
               

                if(flag==true) drawCells(renderer);
                
                
                
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }

                    if (event.type == SDL_KEYDOWN) {
                        if(event.key.keysym.sym == SDLK_g){
                            dfs(renderer, grid_stack, visited);
                            flag = false;
                        }

                         if(event.key.keysym.sym == SDLK_s){
                            if(flag==true){
                                std::cout << "no maze generated!" << std::endl;
                                

                            }
                            else{
                               std::vector<std::string> path = bfs();
                               auto color =  SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
                                for(auto& p : path){
                                    changeCellColor(renderer, color, 5, p); // change cells to yellow
                                }

                                std:: vector<std::string> steps = findPath(pred);

                                color = SDL_SetRenderDrawColor(renderer, 160, 32, 40, SDL_ALPHA_OPAQUE);
                                reverse(steps.begin(), steps.end());
                                for(auto s : steps){
                                    changeCellColor(renderer, color, 25, s); // change cells to purple
                                }

                                color = SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
                                changeCellColor(renderer, color, 100, "0x0"); //change starting cell color to red

                                color = SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                                changeCellColor(renderer, color, 100, "21x29"); //change finishing cell color to green
                                    

                        }

                    }
                }
                
            }
            
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        
    }
    SDL_Quit();
    return 0;
    }
}
