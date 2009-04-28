// $Id: Pathfinding.cpp 4739 2009-04-28 18:58:13Z OLiver $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "Node.h"

#include "GameWorld.h"
#include "noRoadNode.h"
#include "VideoDriverWrapper.h"
#include "Random.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

void Node::Clear()
{
	for(unsigned i = 0;i<6;++i)
		delete next[i];
}

void FlagNode::Clear()
{
	for(unsigned i = 0;i<6;++i)
		delete next[i];
}

unsigned char * GameWorldViewer::FindPath(const bool boat_road,const int x_start,const int y_start, const int x_dest, const int y_dest,const unsigned char playerid)
{
	unsigned nodes_count = 0;
	//unsigned pathtime = VideoDriverWrapper::inst().GetTickCount();

	memset(handled_nodes,0,GetWidth()*GetHeight()*sizeof(unsigned short));
	list<Node*> todo;

	// Anfangsknoten einfgen
	Node start(x_start,y_start,0,0,0);
	todo.push_back(&start);


	while(1)
	{
		// Liste leer und kein Ziel erreicht --> kein Weg
		if(!todo.size())
			return 0;

		// Knoten mit den wenigsten Wegkosten zum Ziel auswäclen
		unsigned shortest_route = 0xFFFFFFFF;

		list<Node*>::iterator best_it;

	
		for(list<Node*>::iterator it = todo.begin(); it.valid(); ++it)
		{
			unsigned new_way = (*it)->way + abs(x_dest - (*it)->x) + abs(y_dest - (*it)->y);
			if(new_way < shortest_route)
			{
				shortest_route = new_way;
				best_it = it;
			}
		}


		// Koordinaten der 6 umliegenden Punkte
		/* id

				1 2
			 0 P 3
				5 4
			*/


		Point coords[6] =
		{
			{ (*best_it)->x -1, (*best_it)->y },
			{ (*best_it)->x-!((*best_it)->y&1), (*best_it)->y-1 },
			{ (*best_it)->x+((*best_it)->y&1), (*best_it)->y-1 },
			{ (*best_it)->x +1, (*best_it)->y },
			{ (*best_it)->x+((*best_it)->y&1), (*best_it)->y+1 },
			{ (*best_it)->x-!((*best_it)->y&1), (*best_it)->y+1 },
		};

		// Knoten in alle 6 Richtungen bilden
		for(unsigned i = 0;i<6;++i)
		{
			//// Nicht über Wasser, Lava, Sümpfe gehen
			//if(!IsNodeToNodeForFigure((*best_it)->x,(*best_it)->y,i,boat_road))
			//	continue;

			if(x_dest == coords[i].x && y_dest == coords[i].y)
			{
				// Ziel erreicht!
				(*best_it)->next[i] = new Node(coords[i].x,coords[i].y,(*best_it)->way+1,*best_it,i);
				unsigned char * route = new unsigned char[(*best_it)->way+2];
				route[(*best_it)->way+1] =255;
				Node * start = (*best_it)->next[i];
				for(int z = start->way-1;start->prev;--z,start = start->prev)
					route[z] =start->dir;

				//LOG.lprintf("Pathfinding: Nodes: %u; Time: %u ms\n",nodes_count,VideoDriverWrapper::inst().GetTickCount()-pathtime);

				return route;
			}

			// Knoten schon auf dem Feld gebildet?
			if(handled_nodes[coords[i].y * GetWidth() + coords[i].x])
				continue;
			// Feld bebaubar?
			if(!RoadAvailable(boat_road,coords[i].x,coords[i].y,i))
				continue;

			// Auch auf unserem Territorium?
			if(!IsPlayerTerritory(coords[i].x,coords[i].y))
				continue;

			(*best_it)->next[i] = new Node(coords[i].x,coords[i].y,(*best_it)->way+1,*best_it,i);
			handled_nodes[coords[i].y * GetWidth() + coords[i].x] = 1;



			todo.push_back((*best_it)->next[i]);
			++nodes_count;
		}

		// Knoten behandelt --> raus aus der todo Liste
		todo.erase(best_it);

	}
}


unsigned char GameWorldGame::FindPath(const noRoadNode * const startflag, const noRoadNode * const goal,bool reverse,unsigned * length,const RoadSegment * const forbidden, const bool use_boat_roads)
{
	if(startflag == goal)
	{
		if(length)
			*length = 0;
		return 0xFF;
	}
	unsigned nodes_count = 0;
	//unsigned pathtime = VideoDriverWrapper::inst().GetTickCount();

	memset(handled_nodes,0,GetWidth()*GetHeight()*sizeof(unsigned short));
	list<FlagNode*> todo;

	// Anfangsknoten einfgen
	FlagNode start(startflag,0,0,0,0);
	todo.push_back(&start);


	while(1)
	{
		// Liste leer und kein Ziel erreicht --> kein Weg
		if(!todo.size())
			return 0xFF;

		// haben wir kein Ziel mehr O.o?
		assert(goal);

		// Knoten mit den wenigsten Wegkosten zum Ziel auswäclen
		unsigned shortest_route = 0xFFFFFFFF;

		list<FlagNode*>::iterator best_it;

		for(list<FlagNode*>::iterator it = todo.begin(); it.valid(); ++it)
		{

			unsigned new_way = (*it)->realway + abs(goal->GetX() - (*it)->flag->GetX()) + abs(goal->GetY() - (*it)->flag->GetY());
			if(new_way < shortest_route)
			{
				shortest_route = new_way;
				best_it = it;
			}
		}



		// Koordinaten der 6 umliegenden Punkte
		/* id

				1 2
			 0 P 3
				5 4
			*/


		// Knoten in alle 6 Richtungen bilden
		for(unsigned i = 0;i<6;++i)
		{
			noRoadNode * neighbour;
			if( (neighbour = (*best_it)->flag->GetNeighbour(i)) )
			{
					// evtl verboten?
				if((*best_it)->flag->routes[i] == forbidden)
					continue;

				// evtl Wasserstraße?
				if((*best_it)->flag->routes[i]->GetRoadType() == RoadSegment::RT_BOAT && !use_boat_roads)
					continue;

				if(neighbour == goal)
				{
					// Ziel erreicht!
					(*best_it)->next[i] = new FlagNode(neighbour,(*best_it)->way+1,*best_it,i,0);

					FlagNode * start = (*best_it)->next[i];

					if(length)
						*length = (*best_it)->realway+(*best_it)->flag->routes[i]->length;


					while(start->prev->prev) start = start->prev;

					return start->dir;
				}

				// Punktezahl (Weg) für diesen Punkt ausrechnen
				unsigned short way_score = (*best_it)->realway+(*best_it)->flag->routes[i]->length;


				// Knoten schon auf dem Feld gebildet, wenn ja muss dieser besser sein
				if(handled_nodes[neighbour->GetY() * GetWidth() + neighbour->GetX()])
				{
					if(way_score >= handled_nodes[neighbour->GetY() * GetWidth() + neighbour->GetX()])
						continue;
				}


				(*best_it)->next[i] = new FlagNode(neighbour,(*best_it)->way+1,*best_it,i,way_score);
				handled_nodes[neighbour->GetY() * GetWidth() + neighbour->GetX()] = way_score;


				todo.push_back((*best_it)->next[i]);
				++nodes_count;
			}
		}

		// Knoten behandelt --> raus aus der todo Liste
		todo.erase(best_it);
	}
}


unsigned char GameWorldGame::FindPathForWare(const noRoadNode * const startflag, const noRoadNode * const goal)
{
	unsigned nodes_count = 0;

	if(!goal)
		return 0xFF;

	memset(handled_nodes,0,GetWidth()*GetHeight()*sizeof(unsigned short));
	list<FlagNode*> todo;



	// Anfangsknoten einfgen
	FlagNode start(startflag,0,0,0,0);
	todo.push_back(&start);


	while(1)
	{
		// Liste leer und kein Ziel erreicht --> kein Weg
		if(!todo.size())
			return 0xFF;

		// Knoten mit den wenigsten Wegkosten zum Ziel auswäclen
		unsigned shortest_route = 0xFFFFFFFF;

		list<FlagNode*>::iterator best_it;

		for(list<FlagNode*>::iterator it = todo.begin(); it.valid(); ++it)
		{

			unsigned new_way = (*it)->realway + abs(goal->GetX() - (*it)->flag->GetX()) 
				+ abs(goal->GetY() - (*it)->flag->GetY());
			if(new_way < shortest_route)
			{
				shortest_route = new_way;
				best_it = it;
			}
		}


		// Koordinaten der 6 umliegenden Punkte
		/* id

				1 2
			 0 P 3
				5 4
			*/


		// Knoten in alle 6 Richtungen bilden
		for(unsigned i = 0;i<6;++i)
		{
			noRoadNode * neighbour;
			if( (neighbour = (*best_it)->flag->GetNeighbour(i)) )
			{
				if(neighbour == goal)
				{
					// Ziel erreicht!
					(*best_it)->next[i] = new FlagNode(neighbour,(*best_it)->way+1,*best_it,i,0);
					FlagNode * start = (*best_it)->next[i];

					while(start->prev->prev) start = start->prev;

					return start->dir;
				}

				// Punktezahl (Weg) für diesen Punkt ausrechnen
				unsigned short way_score = (*best_it)->realway+(*best_it)->flag->routes[i]->length+(*best_it)->flag->GetPunishmentPoints(i);


				// Knoten schon auf dem Feld gebildet, wenn ja muss dieser besser sein
				if(handled_nodes[neighbour->GetY() * GetWidth() + neighbour->GetX()])
				{
					if(way_score >= handled_nodes[neighbour->GetY() * GetWidth() + neighbour->GetX()])
						continue;
				}



				(*best_it)->next[i] = new FlagNode(neighbour,(*best_it)->way+1,*best_it,i,way_score);
				handled_nodes[neighbour->GetY() * GetWidth() + neighbour->GetX()] = way_score;


				todo.push_back((*best_it)->next[i]);
				++nodes_count;
			}
		}

		// Knoten behandelt --> raus aus der todo Liste
		todo.erase(best_it);
	}
}

unsigned char GameWorldBase::FindFreePath(const int x_start,const int y_start, const int x_dest, const int y_dest,unsigned max_route,const bool random_route)
{
	unsigned nodes_count = 0;
	//unsigned pathtime = VideoDriverWrapper::inst().GetTickCount();

	memset(handled_nodes,0,GetWidth()*GetHeight()*sizeof(unsigned short));
	list<Node*> todo;

	// Anfangsknoten einfgen
	Node start(x_start,y_start,0,0,0);
	todo.push_back(&start);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    

	while(1)
	{
		// Liste leer und kein Ziel erreicht --> kein Weg
		if(!todo.size())
			return 0xFF;

		// Knoten mit den wenigsten Wegkosten zum Ziel auswäclen
		unsigned shortest_route = 0xFFFFFFFF;

		list<Node*>::iterator best_it;

		for(list<Node*>::iterator it = todo.begin(); it.valid(); ++it)
		{
			unsigned new_way = (*it)->way + max(abs(x_dest - (*it)->x),abs(y_dest - (*it)->y));
			if(new_way < shortest_route)
			{
				shortest_route = new_way;
				best_it = it;
			}
		}

		// Koordinaten der 6 umliegenden Punkte
		/* id

				1 2
			 0 P 3
				5 4
			*/

		Point coords[6] =
		{
			{ (*best_it)->x -1, (*best_it)->y },
			{ (*best_it)->x-!((*best_it)->y&1), (*best_it)->y-1 },
			{ (*best_it)->x+((*best_it)->y&1), (*best_it)->y-1 },
			{ (*best_it)->x +1, (*best_it)->y },
			{ (*best_it)->x+((*best_it)->y&1), (*best_it)->y+1 },
			{ (*best_it)->x-!((*best_it)->y&1), (*best_it)->y+1 },
		};

		// Bei Zufälliger Richtung anfangen (damit man nicht immer denselben Weg geht, besonders für die Soldaten wichtig)
		unsigned start = random_route?RANDOM.Rand("pf",__LINE__,y_start*GetWidth()+x_start,6):0;


		// Knoten in alle 6 Richtungen bilden
		for(unsigned z = start;z<start+6;++z)
		{
			unsigned i = z%6;

			// Nicht über den Kartenrand lesen!
			if(coords[i].x >= GetWidth() || coords[i].x < 0 || 
				coords[i].y >= GetHeight() || coords[i].y 
				< 0)
				continue;

			// Nicht über Wasser, Lava, Sümpfe gehen
			if(!IsNodeToNodeForFigure((*best_it)->x,(*best_it)->y,i))
				continue;

			if(x_dest == coords[i].x && y_dest == coords[i].y)
			{
				(*best_it)->next[i] = new Node(coords[i].x,coords[i].y,(*best_it)->way+1,*best_it,i);
				Node * start = (*best_it)->next[i];

				while(start->prev->prev) start = start->prev;

				return start->dir;
			}

			// Maximale Strecke schon erreicht?
			if((*best_it)->way == max_route)
				continue;

			// Knoten schon auf dem Feld gebildet
			if(handled_nodes[coords[i].y * GetWidth() + coords[i].x])
				continue;

			// Feld passierbar?
			noBase * obj = GetNode(coords[i].x,coords[i].y).obj;
			if(obj)
			{
				if(obj->GetType() == NOP_BUILDING ||
				obj->GetGOT() == GOT_BUILDINGSITE ||
				obj->GetGOT() == GOT_EXTENSION ||
				obj->GetGOT() == GOT_GRANITE || 
				obj->GetType() == NOP_OBJECT ||
				obj->GetGOT() == GOT_FIRE ||
				obj->GetGOT() == GOT_GRANITE)
				 continue;
			}


			if(GetNode(coords[i].x,coords[i].y).reserved)
				continue;

			(*best_it)->next[i] = new Node(coords[i].x,coords[i].y,(*best_it)->way+1,*best_it,i);
			handled_nodes[coords[i].y * GetWidth() + coords[i].x] = 1;


			todo.push_back((*best_it)->next[i]);
			++nodes_count;
		}

		// Knoten behandelt --> raus aus der todo Liste
		todo.erase(best_it);

	}
}
