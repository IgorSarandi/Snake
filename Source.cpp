#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <windows.h>
#include <algorithm>

#include "Body.h"
#include "Border.h"
#include "Field.h"
#include "Fruit.h"
#include "Head.h"
#include "Objects.h"
#include "Wall.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::pair;
using std::vector;
using std::random_device;
using std::mt19937;


const int X = 24;//height
const int Y = 60;//width
const int minWalls = 10;
const int maxWalls = 30;
const int maxlevel = 3;
int speed = 300;
int level = 0;
int limit = 1000;

vector<int> sequence;

pair<vector<vector<Objects*>>, vector<vector<pair<char, int>>>> Initialize();
string Input();
void Logic(pair<vector<vector<Objects*>>, vector<vector<pair<char, int>>>>&, string key);
void Draw(vector<vector<pair<char, int>>>);
void MakeLevel();

int main()
{
	for (int i = 0; i < maxlevel; i++)
	{
		system("cls");
		MakeLevel();

		pair<vector<vector<Objects*>>, vector<vector<pair<char, int>>>> match = Initialize();

		while (Head::GetScore() < limit)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
			string key = Input();
			Logic(match, key);
			Draw(match.second);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));//1 second
	}
	return 0;
}

void MakeLevel()
{
	level++;
	speed = speed - 100;
	if (level == 2)
		limit = 3000;
	else if (level == 3)
		limit = 100000;
}

template <typename T, typename U>
inline bool Comparator(const vector<T*>& vec, std::pair<U, U> p)
{
	for (auto a : vec)
	{
		if (a->GetPosition() == p) return 1;
	}
	return 0;
}


template <typename T, typename U>
inline auto GetComparator(const vector<T*>& vec, std::pair<U, U> p)
{
	for (auto a : vec)
	{
		if (a->GetPosition() == p)
		{
			auto it = std::find(vec.begin(), vec.end(), a);
			return it;
		}
	}
	return vec.end();
}

pair<vector<vector<Objects*>>, vector<vector<pair<char, int>>>> Initialize()
{
	int total_border = (X * 2) + (Y * 2) - 4;
	int total_fields = (X - 2) * (Y - 2);
	int total = X * Y;
	sequence.reserve(total);
	for (int i = Y; i < total - Y; i++)
	{
		if ((i % Y > 0) && (i % Y < Y - 1)) sequence.push_back(i);
	}

	vector<Objects*> _borders;
	vector<Objects*> _fields;
	vector<Objects*> _head;
	vector<Objects*> _body;
	vector<Objects*> _fruits;
	vector<Objects*> _walls;

	vector<vector<Objects*>> _objects;
	vector<vector<pair<char, int>>> table;
	///char is figure, int is color

	table.resize(X);
	for (int i = 0; i < (int)table.size(); i++)
		table[i].resize(Y);

	Border* border = new Border[total_border]();//make 4 borders up, right, left, down
	Field* field = new Field[total_fields]();

	_borders.reserve(total_border);
	_fields.reserve(total_fields);

	int temp1 = 0;
	int temp2 = 0;

	for (int i = 0; i < X; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			if ((i == 0 || i == X - 1) ||
				(j == 0 && i > 0 && i < X - 1) ||
				(j == Y - 1 && i > 0 && i < X - 1))
			{
				border[temp1].SetPosition(i, j);
				table[i][j] = border[temp1].Figure();

				_borders.push_back(&border[temp1]);

				temp1++;
			}
			else
			{
				field[temp2].SetPosition(i, j);
				table[i][j] = field[temp2].Figure();

				_fields.push_back(&field[temp2]);

				temp2++;
			}
		}
	}

	random_device rd;
	mt19937 g(rd());
	shuffle(sequence.begin(), sequence.end(), g);
	copy(sequence.begin(), sequence.end(), sequence.begin());

	std::uniform_real_distribution<double> rand(minWalls, maxWalls);
	int walls_count = (int)rand(g);

	for (int i = 0; i < (int)sequence.size(); i++)
	{
		int k = sequence[i] / Y;
		int m = sequence[i] - Y * k;

		if (i < walls_count)
		{
			Wall* wall = new Wall();
			_walls.push_back(wall);
			wall->SetPosition(k, m);
			table[k][m] = wall->Figure();
		}
		else
		{
			int check = 0;
			vector<Objects*> check_wall;
			check_wall.reserve(4);

			auto a = GetComparator(_walls, std::make_pair(k + 1, m));
			if(a != _walls.end())
			{
				check_wall.push_back(*a);
				check++;
			}

			a = GetComparator(_walls, std::make_pair(k - 1, m));
			if (a != _walls.end())
			{
				check_wall.push_back(*a);
				check++;
			}

			a = GetComparator(_walls, std::make_pair(k, m + 1));
			if (a != _walls.end())
			{
				check_wall.push_back(*a);
				check++;
			}

			a = GetComparator(_walls, std::make_pair(k, m - 1));
			if (a != _walls.end())
			{
				check_wall.push_back(*a);
				check++;
			}

			if(check > 2)
			{
				shuffle(check_wall.begin(), check_wall.end(), g);
				copy(check_wall.begin(), check_wall.end(), check_wall.begin());
				if (check == 3)
				{
					auto it = std::find(_walls.begin(), _walls.end(), check_wall[0]);
					table[(*it)->GetPosition().first][(*it)->GetPosition().second] = (*_fields.begin())->Figure();
					(*it)->Destroy();
					_walls.erase(it);
				}
				else if (check == 4)
				{
					auto it1 = std::find(_walls.begin(), _walls.end(), check_wall[0]);
					auto it2 = std::find(_walls.begin(), _walls.end(), check_wall[1]);
					table[(*it1)->GetPosition().first][(*it1)->GetPosition().second] = (*_fields.begin())->Figure();
					table[(*it2)->GetPosition().first][(*it2)->GetPosition().second] = (*_fields.begin())->Figure();
					(*it1)->Destroy();
					(*it2)->Destroy();
					_walls.erase(it1);
					_walls.erase(it2);
				}
			}
		}
	}

	int k = X / 2;
	int m = Y / 2;
	if (table[k][m] == (*_walls.begin())->Figure())
	{
		for (auto a : _walls)
		{
			auto it = std::find(_walls.begin(), _walls.end(), a);
			(*it)->Destroy();
			_walls.erase(it);
		}
	}
	Head& head = Head::getSingleton();
	head.SetPosition(k, m);
	table[k][m] = head.Figure();
	_head.push_back(&head);

	_objects.push_back(_borders);
	_objects.push_back(_fields);
	_objects.push_back(_head);
	_objects.push_back(_body);
	_objects.push_back(_fruits);
	_objects.push_back(_walls);

	pair<vector<vector<Objects*>>, vector<vector<pair<char, int>>>> match;
	match.first = _objects;
	match.second = table;

	return match;
}

string Input()
{
	if(GetAsyncKeyState(87) //is W
		|| GetAsyncKeyState(119)) // is w
		return "UP";
	else if (GetAsyncKeyState(65) // is A
		|| GetAsyncKeyState(97)) // is a
		return "LEFT";
	else if (GetAsyncKeyState(83) // is S
		|| GetAsyncKeyState(115))//is s
		return "DOWN";
	else if (GetAsyncKeyState(68) // is D
		|| GetAsyncKeyState(100)) // is d
		return "RIGHT";
	else if (GetAsyncKeyState(VK_ESCAPE)) // is Esc
		return "END";
	else
		return "NULL";
}

void Logic(pair<vector<vector<Objects*>>, vector<vector<pair<char, int>>>>& objects, string key = "NULL")
{
	vector<Border*> _borders;
	vector<Field*> _fields;
	vector<Body*> _body;
	vector<Fruit*> _fruits;
	vector<Wall*> _walls;

	std::transform(objects.first[0].begin(), objects.first[0].end(), std::back_inserter(_borders),
		[](Objects* obj) {return dynamic_cast<Border*>(obj); });
	std::transform(objects.first[1].begin(), objects.first[1].end(), std::back_inserter(_fields),
		[](Objects* obj) {return dynamic_cast<Field*>(obj); });

	Head* _Head = nullptr;
	try
	{
		if (objects.first[2].size() == 0)
			throw "Snake is crashed";
		_Head = dynamic_cast<Head*>(objects.first[2][0]);
	}
	catch (const char* msg)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));//0.5 seconds
		system("cls");
		std::cerr << msg << endl;
		std::cerr << "Your SCORE is: " << Head::GetScore() << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));//0.5 seconds
		system("pause");
		quick_exit(1);
	}

	std::transform(objects.first[3].begin(), objects.first[3].end(), std::back_inserter(_body),
		[](Objects* obj) {return dynamic_cast<Body*>(obj); });
	std::transform(objects.first[4].begin(), objects.first[4].end(), std::back_inserter(_fruits),
		[](Objects* obj) {return dynamic_cast<Fruit*>(obj); });
	std::transform(objects.first[5].begin(), objects.first[5].end(), std::back_inserter(_walls),
		[](Objects* obj) {return dynamic_cast<Wall*>(obj); });

	///Head moves
	int old_dir = _Head->GetDirection().second;
	bool body_move = true;

	if (key == "UP" && old_dir != 2)
	{
		_Head->SetDirection(std::make_pair(_Head->GetDirection().second, 1));
		int k = _Head->GetPosition().first;
		int m = _Head->GetPosition().second;
		pair<char, int> obj = objects.second[k - 1][m];


		if (obj == (*_fields.begin())->Figure())
		{
			k = _Head->GetPosition().first - 1;
			m = _Head->GetPosition().second;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();
			objects.second[k + 1][m] = (*_fields.begin())->Figure();
		}
		else if (obj == (*_borders.begin())->Figure())
		{
			k = X - 2;
			m = _Head->GetPosition().second;
			pair<char, int> new_obj = objects.second[k][m];
			if (new_obj == (*_fields.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[1][m] = (*_fields.begin())->Figure();
			}
			else if (new_obj == (*_fruits.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(1, m);
				objects.second[1][m] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (new_obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (new_obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (obj == (*_fruits.begin())->Figure())
		{
			k = _Head->GetPosition().first - 1;
			m = _Head->GetPosition().second;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();

			Body* body = new Body();
			objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
			_body.insert(_body.begin(), body);//вставка в начало
			body->SetPosition(k + 1, m);
			objects.second[k + 1][m] = body->Figure();
			body_move = false;
			_Head->Eat();

			for (auto a : _fruits)
			{
				a->Destroy();
				auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
				objects.first[4].erase(it2);
			}
		}
		else if (obj == (*_walls.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
		else if (obj == (*_body.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
	}
	else if (key == "DOWN" && old_dir != 1)
	{
		_Head->SetDirection(std::make_pair(_Head->GetDirection().second, 2));
		int k = _Head->GetPosition().first;
		int m = _Head->GetPosition().second;
		pair<char, int> obj = objects.second[k + 1][m];

		if (obj == (*_fields.begin())->Figure())
		{
			k = _Head->GetPosition().first + 1;
			m = _Head->GetPosition().second;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();
			objects.second[k - 1][m] = (*_fields.begin())->Figure();
		}
		else if (obj == (*_borders.begin())->Figure())
		{
			k = 1;
			m = _Head->GetPosition().second;
			pair<char, int> new_obj = objects.second[k][m];
			if (new_obj == (*_fields.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[X - 2][m] = (*_fields.begin())->Figure();
			}
			else if (new_obj == (*_fruits.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(X - 2, m);
				objects.second[X - 2][m] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (new_obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (new_obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (obj == (*_fruits.begin())->Figure())
		{
			k = _Head->GetPosition().first + 1;
			m = _Head->GetPosition().second;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();

			Body* body = new Body();
			objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
			_body.insert(_body.begin(), body);//вставка в начало
			body->SetPosition(k - 1, m);
			objects.second[k - 1][m] = body->Figure();
			body_move = false;
			_Head->Eat();

			for (auto a : _fruits)
			{
				a->Destroy();
				auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
				objects.first[4].erase(it2);
			}
		}
		else if (obj == (*_walls.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
		else if (obj == (*_body.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
	}
	else if (key == "LEFT" && old_dir != 4)
	{
		_Head->SetDirection(std::make_pair(_Head->GetDirection().second, 3));
		int k = _Head->GetPosition().first;
		int m = _Head->GetPosition().second;
		pair<char, int> obj = objects.second[k][m - 1];

		if (obj == (*_fields.begin())->Figure())
		{
			k = _Head->GetPosition().first;
			m = _Head->GetPosition().second - 1;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();
			objects.second[k][m + 1] = (*_fields.begin())->Figure();
		}
		else if (obj == (*_borders.begin())->Figure())
		{
			k = _Head->GetPosition().first;
			m = Y - 2;
			pair<char, int> new_obj = objects.second[k][m];
			if (new_obj == (*_fields.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[k][1] = (*_fields.begin())->Figure();
			}
			else if (new_obj == (*_fruits.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(k, 1);
				objects.second[k][1] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (new_obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (new_obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (obj == (*_fruits.begin())->Figure())
		{
			k = _Head->GetPosition().first;
			m = _Head->GetPosition().second - 1;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();

			Body* body = new Body();
			objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
			_body.insert(_body.begin(), body);//вставка в начало
			body->SetPosition(k, m + 1);
			objects.second[k][m + 1] = body->Figure();
			body_move = false;
			_Head->Eat();

			for (auto a : _fruits)
			{
				a->Destroy();
				auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
				objects.first[4].erase(it2);
			}
		}
		else if (obj == (*_walls.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
		else if (obj == (*_body.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
	}
	else if (key == "RIGHT" && old_dir != 3)
	{
		_Head->SetDirection(std::make_pair(_Head->GetDirection().second, 4));
		int k = _Head->GetPosition().first;
		int m = _Head->GetPosition().second;
		pair<char, int> obj = objects.second[k][m + 1];

		if (obj == (*_fields.begin())->Figure())
		{
			k = _Head->GetPosition().first;
			m = _Head->GetPosition().second + 1;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();
			objects.second[k][m - 1] = (*_fields.begin())->Figure();
		}
		else if (obj == (*_borders.begin())->Figure())
		{
			k = _Head->GetPosition().first;
			m = 1;
			pair<char, int> new_obj = objects.second[k][m];
			if (new_obj == (*_fields.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[k][Y - 2] = (*_fields.begin())->Figure();
			}
			else if (new_obj == (*_fruits.begin())->Figure())
			{
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(k, Y - 2);
				objects.second[k][Y - 2] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (new_obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (new_obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (obj == (*_fruits.begin())->Figure())
		{
			k = _Head->GetPosition().first;
			m = _Head->GetPosition().second + 1;
			_Head->SetPosition(k, m);
			objects.second[k][m] = _Head->Figure();

			Body* body = new Body();
			objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
			_body.insert(_body.begin(), body);//вставка в начало
			body->SetPosition(k, m - 1);
			objects.second[k][m - 1] = body->Figure();
			body_move = false;
			_Head->Eat();

			for (auto a : _fruits)
			{
				a->Destroy();
				auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
				objects.first[4].erase(it2);
			}
		}
		else if (obj == (*_walls.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
		else if (obj == (*_body.begin())->Figure())
		{
			_Head->Destroy();
			objects.first[2].erase(objects.first[2].begin());
		}
	}
	else if (key == "END")
	{
		quick_exit(0);
	}
	else
	{
		int dir = _Head->GetDirection().second;

		if (dir == 1 && old_dir != 2)//up
		{
			_Head->SetDirection(std::make_pair(dir, 1));
			int k = _Head->GetPosition().first;
			int m = _Head->GetPosition().second;
			pair<char, int> obj = objects.second[k - 1][m];

			if (obj == (*_fields.begin())->Figure())
			{
				k = _Head->GetPosition().first - 1;
				m = _Head->GetPosition().second;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[k + 1][m] = (*_fields.begin())->Figure();
			}
			else if (obj == (*_borders.begin())->Figure())
			{
				k = X - 2;
				m = _Head->GetPosition().second;
				pair<char, int> new_obj = objects.second[k][m];
				if (new_obj == (*_fields.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();
					objects.second[1][m] = (*_fields.begin())->Figure();
				}
				else if (new_obj == (*_fruits.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();

					Body* body = new Body();
					objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
					_body.insert(_body.begin(), body);//вставка в начало
					body->SetPosition(1, m);
					objects.second[1][m] = body->Figure();
					body_move = false;
					_Head->Eat();

					for (auto a : _fruits)
					{
						a->Destroy();
						auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
						objects.first[4].erase(it2);
					}
				}
				else if (new_obj == (*_walls.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
				else if (new_obj == (*_body.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
			}
			else if (obj == (*_fruits.begin())->Figure())
			{
				k = _Head->GetPosition().first - 1;
				m = _Head->GetPosition().second;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(k + 1, m);
				objects.second[k + 1][m] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (dir == 2 && old_dir != 1)//down
		{
			_Head->SetDirection(std::make_pair(dir, 2));
			int k = _Head->GetPosition().first;
			int m = _Head->GetPosition().second;
			pair<char, int> obj = objects.second[k + 1][m];

			if (obj == (*_fields.begin())->Figure())
			{
				k = _Head->GetPosition().first + 1;
				m = _Head->GetPosition().second;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[k - 1][m] = (*_fields.begin())->Figure();
			}
			else if (obj == (*_borders.begin())->Figure())
			{
				k = 1;
				m = _Head->GetPosition().second;
				pair<char, int> new_obj = objects.second[k][m];
				if (new_obj == (*_fields.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();
					objects.second[X - 2][m] = (*_fields.begin())->Figure();
				}
				else if (new_obj == (*_fruits.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();

					Body* body = new Body();
					objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
					_body.insert(_body.begin(), body);//вставка в начало
					body->SetPosition(X - 2, m);
					objects.second[X - 2][m] = body->Figure();
					body_move = false;
					_Head->Eat();

					for (auto a : _fruits)
					{
						a->Destroy();
						auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
						objects.first[4].erase(it2);
					}
				}
				else if (new_obj == (*_walls.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
				else if (new_obj == (*_body.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
			}
			else if (obj == (*_fruits.begin())->Figure())
			{
				k = _Head->GetPosition().first + 1;
				m = _Head->GetPosition().second;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(k - 1, m);
				objects.second[k - 1][m] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (dir == 3 && old_dir != 4)// left
		{
			_Head->SetDirection(std::make_pair(dir, 3));
			int k = _Head->GetPosition().first;
			int m = _Head->GetPosition().second;
			pair<char, int> obj = objects.second[k][m - 1];

			if (obj == (*_fields.begin())->Figure())
			{
				k = _Head->GetPosition().first;
				m = _Head->GetPosition().second - 1;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[k][m + 1] = (*_fields.begin())->Figure();
			}
			else if (obj == (*_borders.begin())->Figure())
			{
				k = _Head->GetPosition().first;
				m = Y - 2;
				pair<char, int> new_obj = objects.second[k][m];
				if (new_obj == (*_fields.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();
					objects.second[k][1] = (*_fields.begin())->Figure();
				}
				else if (new_obj == (*_fruits.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();

					Body* body = new Body();
					objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
					_body.insert(_body.begin(), body);//вставка в начало
					body->SetPosition(k, 1);
					objects.second[k][1] = body->Figure();
					body_move = false;
					_Head->Eat();

					for (auto a : _fruits)
					{
						a->Destroy();
						auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
						objects.first[4].erase(it2);
					}
				}
				else if (new_obj == (*_walls.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
				else if (new_obj == (*_body.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
			}
			else if (obj == (*_fruits.begin())->Figure())
			{
				k = _Head->GetPosition().first;
				m = _Head->GetPosition().second - 1;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(k, m + 1);
				objects.second[k][m + 1] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
		else if (dir == 4 && old_dir != 3)//right
		{
			_Head->SetDirection(std::make_pair(dir, 4));
			int k = _Head->GetPosition().first;
			int m = _Head->GetPosition().second;
			pair<char, int> obj = objects.second[k][m + 1];

			if (obj == (*_fields.begin())->Figure())
			{
				k = _Head->GetPosition().first;
				m = _Head->GetPosition().second + 1;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();
				objects.second[k][m - 1] = (*_fields.begin())->Figure();
			}
			else if (obj == (*_borders.begin())->Figure())
			{
				k = _Head->GetPosition().first;
				m = 1;
				pair<char, int> new_obj = objects.second[k][m];
				if (new_obj == (*_fields.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();
					objects.second[k][Y - 2] = (*_fields.begin())->Figure();
				}
				else if (new_obj == (*_fruits.begin())->Figure())
				{
					_Head->SetPosition(k, m);
					objects.second[k][m] = _Head->Figure();

					Body* body = new Body();
					objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
					_body.insert(_body.begin(), body);//вставка в начало
					body->SetPosition(k, Y - 2);
					objects.second[k][Y - 2] = body->Figure();
					body_move = false;
					_Head->Eat();

					for (auto a : _fruits)
					{
						a->Destroy();
						auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
						objects.first[4].erase(it2);
					}
				}
				else if (new_obj == (*_walls.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
				else if (new_obj == (*_body.begin())->Figure())
				{
					_Head->Destroy();
					objects.first[2].erase(objects.first[2].begin());
				}
			}
			else if (obj == (*_fruits.begin())->Figure())
			{
				k = _Head->GetPosition().first;
				m = _Head->GetPosition().second + 1;
				_Head->SetPosition(k, m);
				objects.second[k][m] = _Head->Figure();

				Body* body = new Body();
				objects.first[3].insert(objects.first[3].begin(), body);//вставка в начало
				_body.insert(_body.begin(), body);//вставка в начало
				body->SetPosition(k, m - 1);
				objects.second[k][m - 1] = body->Figure();
				body_move = false;
				_Head->Eat();

				for (auto a : _fruits)
				{
					a->Destroy();
					auto it2 = std::find(objects.first[4].begin(), objects.first[4].end(), a);
					objects.first[4].erase(it2);
				}
			}
			else if (obj == (*_walls.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
			else if (obj == (*_body.begin())->Figure())
			{
				_Head->Destroy();
				objects.first[2].erase(objects.first[2].begin());
			}
		}
	}

	if (!body_move)
	{
		for (auto a : _body)
		{
			if (a == _body[0]) 
			{
				a->SetParent(_Head);
				a->SetDirection(std::make_pair(_Head->GetDirection().first, _Head->GetDirection().first));
			}
			else if(a==_body[1])
			{
				a->SetParent(_body[0]);
				break;
			}
		}
	}
	else
	{
		for (auto a : _body)
		{
			int k = a->GetPosition().first;
			int m = a->GetPosition().second;

			int dir = a->GetParent()->GetDirection().first;
			a->SetDirection(std::make_pair(a->GetDirection().second, dir));

			if (dir == 1)
			{
				if (objects.second[k - 1][m] == (*_fields.begin())->Figure())
				{
					a->SetPosition(k - 1, m);
					objects.second[k - 1][m] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
				else if (objects.second[k - 1][m] == (*_borders.begin())->Figure())
				{
					a->SetPosition(X - 2, m);
					objects.second[X - 2][m] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
			}
			else if(dir == 2)
			{
				if (objects.second[k + 1][m] == (*_fields.begin())->Figure())
				{
					a->SetPosition(k + 1, m);
					objects.second[k + 1][m] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
				else if (objects.second[k + 1][m] == (*_borders.begin())->Figure())
				{
					a->SetPosition(1, m);
					objects.second[1][m] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
			}
			else if (dir == 3)
			{
				if (objects.second[k][m - 1] == (*_fields.begin())->Figure())
				{
					a->SetPosition(k, m - 1);
					objects.second[k][m - 1] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
				else if (objects.second[k][m - 1] == (*_borders.begin())->Figure())
				{
					a->SetPosition(k, Y - 2);
					objects.second[k][Y - 2] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
			}
			else if (dir == 4)
			{
				if (objects.second[k][m + 1] == (*_fields.begin())->Figure())
				{
					a->SetPosition(k, m + 1);
					objects.second[k][m + 1] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
				else if (objects.second[k][m + 1] == (*_borders.begin())->Figure())
				{
					a->SetPosition(k, 1);
					objects.second[k][1] = a->Figure();
					objects.second[k][m] = (*_fields.begin())->Figure();
				}
			}
		}
	}

	if (!_fruits.size())
	{
		random_device rd;
		mt19937 g(rd());
		shuffle(sequence.begin(), sequence.end(), g);
		copy(sequence.begin(), sequence.end(), sequence.begin());//перемешиваем все ячейки игры, кроме обьектов типа border

		for (int i = 0; i < (int)sequence.size(); i++)
		{
			bool b = false;
			int k = sequence[i] / Y;
			int m = sequence[i] - Y * k;
			std::pair <int, int> p {k, m};
			if (_Head->GetPosition() == p)
			{
				continue;
			}
			b = Comparator(_body, p);
			if (b)
			{
				continue;
			}
			else
			{
				Fruit* fruit = new Fruit();
				objects.first[4].push_back(fruit);
				fruit->SetPosition(k, m);
				objects.second[k][m] = fruit->Figure();
				break;
			}
		}
	}
}


inline void setcur(int x, int y)//установка курсора на позицию  x y 
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

void Draw(vector<vector<pair<char, int>>> objects)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	setcur(0, 0);
	for (int i = 0; i < (int)objects.size(); i++)
	{
		for (int j = 0; j < (int)objects[i].size(); j++)
		{
			SetConsoleTextAttribute(hConsole, objects[i][j].second);
			cout << objects[i][j].first;
		}
		cout << endl;
	}
	SetConsoleTextAttribute(hConsole, 15);
	cout << "Score: ";
	SetConsoleTextAttribute(hConsole, 12);
	cout << Head::GetScore() << "\\";
	if (limit == 100000)
		cout << "UNLIMITED";
	else
		cout << limit;
	SetConsoleTextAttribute(hConsole, 15);
	cout << "\t\tLevel: ";
	SetConsoleTextAttribute(hConsole, 12);
	cout << level << "\\" << maxlevel;
	SetConsoleTextAttribute(hConsole, 15);
	cout << "\t\tSpeed: ";
	SetConsoleTextAttribute(hConsole, 12);
	if (speed == 200)
		cout << "x1";
	else if (speed == 100)
		cout << "x2";
	else
		cout << "MAX";
	setcur(0, 0);
}