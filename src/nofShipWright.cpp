#include "main.h"
#include "nofShipWright.h"
#include "nobShipYard.h"
#include "GameWorld.h"
#include "SerializedGameData.h"

nofShipWright::nofShipWright(const unsigned short x, const unsigned short y,const unsigned char player,nobUsual * workplace) 
		: nofWorkman(JOB_SHIPWRIGHT,x,y,player,workplace), ship_x(0xFFFF), ship_y(0xFFFF)
{}

void nofShipWright::HandleDerivedEvent(const unsigned int id)
{
	switch(state)
	{
	case STATE_WAITING1:
		{
			// Herausfinden, was der Schiffsbauer als nächstes bauen soll
			if(dynamic_cast<nobShipYard*>(workplace)->GetMode() == nobShipYard::BOATS)
				// in Handwerksmanier Boote herstellen
				nofWorkman::HandleStateWaiting1();
			else
			{
				// Besitze ich noch ein Schiff, was gebaut werden muss?
				noBase * obj = gwg->GetNode(ship_x,ship_y).obj;
			}
		} break;
	case STATE_WORK:
		{
			// Sind wir an unserem Arbeitsplatz (dem Gebäude), wenn wir die Arbeit beendet haben, bauen wir nur Boote,
			// ansonsten sind wir an unserem Schiff und bauen große Schiffe
			if(workplace->GetX() == x && workplace->GetY() == y)
				// Boote bauen
				nofWorkman::HandleStateWork();
			else
			{

			}
		} break;
	case STATE_WAITING2:
		{
			// Hier ist die Sache klar, dieser State kann nur bei Handwerkern vorkommen
			nofWorkman::HandleStateWaiting2();
		} break;
    default:
		break;
	}
}

nofShipWright::nofShipWright(SerializedGameData * sgd, const unsigned obj_id) 
: nofWorkman(sgd,obj_id),
ship_x(sgd->PopUnsignedShort()),
ship_y(sgd->PopUnsignedShort())
{
}

void nofShipWright::Serialize(SerializedGameData *sgd) const
{
	nofWorkman::Serialize(sgd);

	sgd->PushUnsignedShort(ship_x);
	sgd->PushUnsignedShort(ship_y);
}