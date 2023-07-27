#include<iostream> 
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>

using namespace std;

#define CURRENT 0
#define MEMORY 1

#define CURRENT_BLOCK ::game_map[CURRENT][y][x]
#define MEMORY_BLOCK ::game_map[MEMORY][y][x]

#define LAVA dict['F']
#define IRON dict['C']
#define OSMIUM dict['D']

#define NULL_DIRECTION(pos) pos.x==-1&&pos.y==-1
#define NULL_POINT {-1,-1}

char game_map[2][105][105] = {0};

int _round = 0, id = 0;
int map_width = 0, map_height = 0;
int pl_x=0,pl_y=0;
int base_x=0,base_y=0;

int min_x=120,max_x=-1;
int min_y=120,max_y=-1;

int health=0,drill=0,attack=0,agility=0,scan=0,battery=0,radar=0;
int cobble=0,iron=0,osmium=0;

map<char,int> dict;

struct POS{
    int x,y;
};

void show_map(){
	
	printf("--ROWSTART--\n");
	for (int y = min_y; y <= max_y; ++y){
		for (int x = min_x; x <= max_x; ++x)
		{
			if(CURRENT_BLOCK=='?') {printf("_"); continue;}
			printf("%c",CURRENT_BLOCK);
		}
		printf(" ");
		for (int x = min_x; x <= max_x; ++x)
		{
			if(!MEMORY_BLOCK) {printf("_"); continue;}
			printf("%c",MEMORY_BLOCK);
		}
		printf("\n");
	}
	printf("--ROWEND--\n");
}

void gather_data(std::ifstream& in)
{

	memset(game_map[CURRENT],sizeof(game_map[CURRENT]),0);

	show_map();

	in >> ::map_width >> ::map_height;
	for (int y = 0; y < map_height; ++y)
		for (int x = 0; x < map_width; ++x)
		{
			in>>CURRENT_BLOCK;
			if(CURRENT_BLOCK!='?'){
				::min_x=min(min_x,x),::min_y=min(min_y,y);
				::max_x=max(max_x,x),::max_y=max(max_y,y);
			}
		}

	for (int y = min_y; y <= max_y; ++y)
		for (int x = min_x; x <= max_x; ++x) if(CURRENT_BLOCK!='?')MEMORY_BLOCK=CURRENT_BLOCK;
	
	in>>pl_x>>pl_y;
	base_y=pl_y,base_x=pl_x;
	in>>health>>drill>>attack>>agility>>scan>>battery;
	in>>cobble>>iron>>osmium;
	//system("cls");
	//show_map();
}

POS last_taken_direction=NULL_POINT;

string path_finding(int _x,int _y){

    bool access_map[105][105]={0};
    POS parents[105][105]={0};
    int val_map[105][105]={0};

    std::queue<POS> qu;

    qu.push({pl_x,pl_y});
    access_map[pl_x][pl_y]=1;

    int dx[4]={0,0,1,-1};
    int dy[4]={1,-1,0,0};

    for (int y = 0; y <= map_height; ++y){
		for (int x = 0; x <= map_width; ++x){
            if(MEMORY_BLOCK=='B') access_map[y][x]=1;
            if(MEMORY_BLOCK=='F') {
                access_map[y][x]=1;
                for(int j=0;j<4;++j) access_map[y+dy[j]][x+dx[j]]=1;
            }
        }
    }

    while (!qu.empty())
    {
        POS top = qu.front();
        qu.pop();

        #define tx top.x
        #define ty top.y

        for(int i=0;i<4;++i){
            if(tx+dx[i]<0 || ty+dy[i]<0) continue;
            if(tx+dx[i]>map_width || ty+dy[i]>map_height) continue;
            if(access_map[ty+dy[i]][tx+dx[i]]) continue;

            parents[ty+dy[i]][tx+dx[i]]={tx,ty};

            val_map[ty+dy[i]][tx+dx[i]]=val_map[ty][tx]+1;
            access_map[ty+dy[i]][tx+dx[i]]=1;
            qu.push({tx+dx[i],ty+dy[i]});
        }

    }

	POS current=parents[_y][_x],last_pos=current;

	printf("%d %d\n",current.x,current.y);

	int j=0;

	while(current.x!=0 && j<150){
		printf("%d %d\n",current.y,current.x);
		last_pos=current;
		current=parents[current.y][current.x];
		++j;
	}

	printf("CURRENT %d %d\n",current.x,current.y);
    printf("PLAYER %d %d\n",pl_x,pl_y);

}

string explore(){
    std::string res;

    if(NULL_DIRECTION(last_taken_direction)){
        last_taken_direction={rand()%int((map_width/10.0f)*9.0f)+int(map_width/10.0f)/2,rand()%int((map_height/10.0f)*9.0f)+int(map_height/10.0f)/2};
    }

    printf("MAP WIDTH %d, MAP HEIGHT %d\n",map_width,map_height);
    printf("LTD %d %d\n", last_taken_direction.x,last_taken_direction.y);

    path_finding(last_taken_direction.x,last_taken_direction.y);
    
}

std::vector<vector<POS>> surroundings(){
    std::vector<vector<POS>> res={vector<POS>(),vector<POS>(),vector<POS>(),vector<POS>(),vector<POS>()};
    for(int y=min_y;y<=max_y;++y){
        for(int x=min_x;y<=max_x;++x){
            if(dict[CURRENT_BLOCK]){
                res[dict[CURRENT_BLOCK]].push_back({x,y});
            }
        }
    }
}

void take_decisions(std::ofstream& out){
    //Explore
    auto surrounding_blocks =  surroundings();
    //if(surrounding_blocks[OSMIUM].size()==0 && surrounding_blocks[IRON].size()==0){
        explore();
    //}

    //Gather
    //Come back
}

int main()
{

    dict['X']=0,dict['C']=1,dict['D']=2,dict['B']=3,dict['F']=4;

	int id = 0;
	std::cout << "enter id: ";
	std::cin >> id;

	int round = 0;

	while (true)
	{
		std::string serverFileName = "game/s" + std::to_string(id) + "_" + std::to_string(round) + 
			".txt";

		std::ifstream input(serverFileName);

		if (input)
		{
            gather_data(input);
			input.close();
			std::string ourFileName = "game/c" + std::to_string(id) + "_" + std::to_string(round) + 
				".txt";
			std::ofstream response(ourFileName);
            take_decisions(response);
			response.close();
			round++;
		}
		else
		{
			//waiting
		}
	}
	return 0;
}