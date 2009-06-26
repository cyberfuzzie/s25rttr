#include "main.h"
#include "nofShipWright.h"
#include "nobShipYard.h"


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
				// Auf Suche nach Schiffsbauplatz gehen
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

