#include <iostream>
#include <fstream>
#include <string>
#include<queue>
#include<map>
using namespace std;

#define CURRENT 0
#define MEMORY 1

#define CURRENT_BLOCK ::game_map[CURRENT][y][x]
#define MEMORY_BLOCK ::game_map[MEMORY][y][x]

char game_map[3][105][105] = {0};

int _round = 0, id = 0;
int map_width = 0, map_height = 0;
int pl_x=0,pl_y=0;
int base_x=0,base_y=0;

int min_x=120,max_x=-1;
int min_y=120,max_y=-1;

int health=0,drill=0,attack=0,agility=0,scan=0,battery=0,radar=0;
int cobble=0,iron=0,osmium=0;

struct UPGRADE {
	char code;
	int iron,oxi;
	int& target_val;
};
//CALLS WITH B
std::vector<UPGRADE> upgrades = {
	{'S',3,0,scan},
	{'M',3,0,agility},
	{'A',3,0,attack},
	{'H',0,1,health},
	{'D',3,0,drill},
	{'R',2,1,radar},
	{'S',6,1,scan},
	{'M',6,1,agility},
	{'A',6,1,attack},
	{'D',6,1,drill}
};


void show_map(){
	
	printf("OSMIUM %d, IRON %d\n",osmium,iron);
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
	in>>health>>drill>>attack>>agility>>scan>>radar>>battery;
	in>>cobble>>iron>>osmium;
	//system("cls");
	//show_map();
}

char MOVES[4]={'U','D','L','R'};

struct block{
	int x, y,type;

	bool operator<(const block& right) const {
		if(type > right.type) return false;
		if(abs(pl_x-x)+abs(pl_y-y)<abs(pl_x-right.x)+abs(pl_y-right.y)) return false;
		return true;
	}
};

std::map<char,int> rocks;

std::deque<char> backtr;

bool GO_HOME=false;

void execute_actions(std::ofstream& out){

	int dx[4]={0,0,1,-1};
	int dy[4]={-1,1,0,0};

	bool access_map[105][105] = {0};
	int parent_map[105][105][2] = {0};
	int value_map[105][105]={0};

	bool danger=false;
	for(int i=0;i<4;++i) if(game_map[CURRENT][pl_y+dy[i]][pl_x+dx[i]]=='F'){
		danger=true;
	}

	if(danger==true){
		char move = 'U';
		if(pl_x<map_width/2) move='R';
		if(pl_y<map_height/2) move='U';
		if(pl_x>map_width/2) move='L';
		if(pl_y>map_height/2) move='U';
		out<<move<<" M "<<move<<'\n';
		return;
	}

	if(GO_HOME==false||battery==true){
		show_map();

		std::priority_queue<block> list;

		for(int y=pl_y-1-scan;y<=min(pl_y+1+scan,map_height);++y)
			for(int x=pl_x-1-scan;x<=min(pl_x+1+scan,map_width);++x){
				if(rocks[CURRENT_BLOCK]){
					list.push({x,y,rocks[CURRENT_BLOCK]});
				} 
				if(CURRENT_BLOCK=='B') access_map[y][x]=1,value_map[y][x]=-1;
				if(CURRENT_BLOCK=='F'){
					access_map[y][x]=1,value_map[y][x]=-1;
					for(int i=0;i<4;++i) access_map[y+dy[i]][x+dx[i]]=1,value_map[y+dy[i]][x+dx[i]]=-1;
				}
			}
				

		if(list.empty()) {
			char move = 'U';
			if(pl_x<map_width/2) move='R';
			if(pl_y<map_height/2) move='U';
			if(pl_x>map_width/2) move='L';
			if(pl_y>map_height/2) move='U';
			out<<move<<' '<<"M "<<move<<'\n';
			return;
		}

		std::priority_queue<block> copy=list;

		while(!copy.empty()){
			const block top = copy.top();
			copy.pop();
			printf("BLOCK: %d %d, type-%d\n",top.x,top.y,top.type);
		}


		std::queue<pair<int,int>> dq;

		dq.push({pl_y,pl_x});

		while(!dq.empty()){

			pair<int,int> pos=dq.front();
			access_map[pos.first][pos.second]=1;
			dq.pop();

			printf("CHECKING PARENT %d %d\n",pos.first,pos.second);

			for(int i=0;i<4;++i){
				printf("CHECKING %d %d\n",pos.first+dy[i],pos.second+dx[i]);
				if(pos.first<max(pl_y-1-scan,0) || pos.second<max(pl_x-1-scan,0)) continue;
				if(pos.first+dy[i]>min(pl_y+1+scan,map_height)) continue;
				if(pos.second+dx[i]>min(pl_x+1+scan,map_width)) continue;
				if(access_map[pos.first+dy[i]][pos.second+dx[i]]) continue;

				parent_map[pos.first+dy[i]][pos.second+dx[i]][0]=pos.first;//y
				parent_map[pos.first+dy[i]][pos.second+dx[i]][1]=pos.second;//x
				value_map[pos.first+dy[i]][pos.second+dx[i]] = value_map[pos.first][pos.second]+1;

				access_map[pos.first+dy[i]][pos.second+dx[i]]=1;
				dq.push({pos.first+dy[i],pos.second+dx[i]});
			}
		}
		printf("PARENT ----------\n");
		for(int y=max(pl_y-1-scan,0);y<=min(pl_y+1+scan,map_height);++y){
			for(int x=max(pl_x-1-scan,0);x<=min(pl_x+1+scan,map_width);++x){
				printf("%d ",parent_map[y][x][0]);
			}
			printf("  ");
			for(int x=max(pl_x-1-scan,0);x<=min(pl_x+1+scan,map_width);++x){
				printf("%d ",parent_map[y][x][1]);
			}
			printf("\n");
		}
		printf("END PARENT ------\n");

		std::pair<int,int> target = {list.top().y,list.top().x},last_target=target,save=target;

		while(!list.empty()){
			if(access_map[list.top().y][list.top().x]==1){
				target={list.top().y,list.top().x};
				printf("FOUND TARGET %d %d\n",target.first,target.second);
				break;
			}
			list.pop();
		}

		printf("TARGET CURR AT %d %d\n",target.first,target.second);
		printf("BOT AT %d %d\n",pl_y,pl_x);

		printf("%d %d\n",parent_map[target.first][target.second][0],parent_map[target.first][target.second][1]);

		while(parent_map[target.first][target.second][0]!=0){
			printf("WHILE %d %d\n",target.first,target.second);

			last_target=target;
			target.first=parent_map[target.first][target.second][0];
			target.second=parent_map[target.first][target.second][1];
		}

		printf("%d %d\n%d %d\n",pl_y,pl_x,last_target.first,last_target.second);

		printf("TO TOUCH %d %d\n",pl_y-last_target.first,pl_x-last_target.second);

		char move = 'U';
		if(pl_x<map_width/2) move='R';
		if(pl_y<map_height/2) move='U';
		if(pl_x>map_width/2) move='L';
		if(pl_y>map_height/2) move='U';

		parent_map[last_target.first][last_target.second][0]=0;
		parent_map[last_target.first][last_target.second][1]=0;

		if(last_target.first-pl_y==-1) move='U';
		if(last_target.first-pl_y==1) move='D';
		if(last_target.second-pl_x==-1) move='L';
		if(last_target.second-pl_x==1) move='R';

		show_map();

		printf("MOVES: ");
		printf("%c ",move);
		out<<move<<' ';

		if(!battery){
			if(game_map[last_target.first][last_target.second]==".")
				backtr.push_front(move);
		} 

		if(game_map[last_target.first][last_target.second]!="." && game_map[last_target.first][last_target.second]!="E"){
			target=save;

			while(parent_map[target.first][target.second][0]!=0){
				last_target=target;
				target.first=parent_map[target.first][target.second][0];
				target.second=parent_map[target.first][target.second][1];
			}
			out<<"M ";

			if(last_target.first-pl_y==-1) move='U';
			if(last_target.first-pl_y==1) move='U';
			if(last_target.second-pl_x==-1) move='L';
			if(last_target.second-pl_x==1) move='R';
			out<<move<<' ';
			printf("M %c ",move);
		}
		printf("\n");
	}
	else{
		if(!backtr.empty()){
			switch (backtr.front())
			{
			case 'U':
				out<<"D M D\n";
				break;
			
			case 'D':
				out<<"U M U\n";
				break;

			case 'L':
				out<<"R M R\n";
				break;

			case 'R':
				out<<"L M L\n";
				break;
			
			default:
				break;
			}
			backtr.pop_front();
			if(backtr.empty()) {out<<"B B";GO_HOME=false;battery=true;printf("BOUGHT BATTERY\n");}
		}
			
	}
}

int main()
{

	rocks['X']=1,rocks['C']=2,rocks['D']=3,rocks['E']=0;

	srand((unsigned) time(NULL));

	std::cout << "enter id: ";
	std::cin >> id;

	while (true)
	{
		std::string serverFileName = "game/s" + std::to_string(id) + "_" + std::to_string(::_round) +
									 ".txt";

		std::ifstream input(serverFileName);

		if (input)
		{
			gather_data(input);

			input.close();
			std::string ourFileName = "game/c" + std::to_string(id) + "_" + std::to_string(::_round) +
									  ".txt";
			std::ofstream response(ourFileName);

			//if(iron>=1&&osmium>=1&&!battery) GO_HOME=true;
			execute_actions(response);
			//response<<MOVES[rand()%5];
			response.close();
			::_round++;
		}
		else
		{
			// waiting
		}
	}
	system("pause");
	return 0;
}
