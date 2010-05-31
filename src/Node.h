// $Id: Node.h 6458 2010-05-31 11:38:51Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef NODE_H_
#define NODE_H_

class noRoadNode;

class Node
{
public:

	Node()
	{
		x = 0;
		y = 0;
		way = 0;
		prev = NULL;
		dir = 0;
		memset(next, 0, sizeof(Node*)*6);
	}

	Node(const int x, const int y, const unsigned way,Node * const prev,const unsigned dir)
	{
		this->x = x;
		this->y = y;
		this->way = way;
		this->prev = prev;
		this->dir = dir;
		memset(next, 0, sizeof(Node*)*6);
	}
	~Node() { Clear(); }

	void Clear();

	int x, y;
	unsigned way;
	unsigned char dir;
	Node * prev;
	Node * next[6];
};

class FlagNode
{
public:

	FlagNode()
	{
		flag = NULL;
		way = 0;
		prev = NULL;
		dir = 0;
		realway = 0;
		memset(next,0,sizeof(FlagNode*)*6);
	}

	FlagNode(const noRoadNode * const flag, const unsigned way,FlagNode * const prev,const unsigned dir,const unsigned realway)
	{
		this->flag = flag;
		this->way = way;
		this->prev = prev;
		this->dir = dir;
		this->realway = realway;
		memset(next,0,sizeof(FlagNode*)*6);
	}
	~FlagNode() { Clear(); }

	void Clear();

	const noRoadNode * flag;
	unsigned way,realway;
	unsigned char dir;
	FlagNode * prev;
	FlagNode * next[6];
};


#endif
