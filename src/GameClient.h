// $Id: GameClient.h 4809 2009-05-04 20:10:11Z OLiver $
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
#ifndef GAMECLIENT_H_
#define GAMECLIENT_H_

#pragma once

#include "Socket.h"

#include "Singleton.h"
#include "GameProtocol.h"
#include "GameClientPlayer.h"
#include "EventManager.h"
#include "GameFiles.h"
#include "GameWorld.h"
#include "GlobalGameSettings.h"
#include <string>

class Window;
class GameClientPlayer;
class WorldManager;
class ClientInterface;

class GameClient : public Singleton<GameClient>
{
public:
	enum ClientState
	{
		CS_STOPPED = 0,
		CS_CONNECT,
		CS_CONFIG,
		CS_GAME
	};

	GameClient(void);
	~GameClient(void);

	void SetInterface(ClientInterface *ci) { this->ci = ci; }
	bool IsHost() const { return clientconfig.host; }
	bool IsSavegame() const { return mapinfo.map_type == MAPTYPE_SAVEGAME; }
	std::string GetGameName() const { return clientconfig.gamename; }

	unsigned char GetPlayerID() const { return playerid; }
	unsigned GetPlayerCount() const { return player_count; }
	GameClientPlayer * GetLocalPlayer() { return players[playerid]; }
	/// Liefert einen Player zurück
	GameClientPlayer * GetPlayer(const unsigned i) { return players[i]; }

	/// Gibt GGS zurück
	const GlobalGameSettings& GetGGS() const { return ggs; }

	bool Connect(const std::string& server, const std::string& password, unsigned char servertyp, unsigned short port, bool host);
	void Run();
	void Stop();

	// Gibt GameWorldViewer zurück (VORLÄUFIG, soll später verschwinden!!)
	GameWorldViewer * QueryGameWorldViewer() const { return static_cast<GameWorldViewer*>(gw); }
	/// Gibt Map-Titel zurück
	const std::string& GetMapTitle() const { return mapinfo.title; }
	/// Gibt Pfad zu der Map zurück
	const std::string& GetMapPath() const  { return clientconfig.mapfilepath; }
	/// Gibt Map-Typ zurück
	const MapType GetMapType() const { return mapinfo.map_type; }


	// Initialisiert und startet das Spiel
	void StartGame(const unsigned random_init);
	/// Wird aufgerufen, wenn das GUI fertig mit Laden ist und es losgehen kann
	void RealStart();

	/// Beendet das Spiel, zerstört die Spielstrukturen
	void ExitGame();

	ClientState GetState() const { return state; }
	unsigned int GetGFNumber() const { return framesinfo.nr; }
	unsigned int GetGFLength() const { return framesinfo.gf_length; }
	unsigned int GetNWFLength() const { return framesinfo.nwf_length; }
	unsigned int GetFrameTime() const { return framesinfo.frame_time; }
	unsigned int GetGlobalAnimation(const unsigned max,unsigned factor_numerator, unsigned factor_denumerator, unsigned offset);
	unsigned int Interpolate(unsigned max_val,EventManager::EventPointer ev);
	int Interpolate(int x1,int x2,EventManager::EventPointer ev);
	/// Gibt Geschwindigkeits-Faktor zurück

	// NC = Network Commands
	void ExecuteNC(const unsigned short nfc_type,const unsigned char player, const void * const data);
	void NC_Nothing();
	void NC_SetFlag(const unsigned short x,const unsigned short y);
	void NC_DestroyFlag(const unsigned short x, const unsigned short y);
	void NC_Road_Build(const bool boat_road,const unsigned short road_start_x, const unsigned short road_start_y, const unsigned char * route, const unsigned short road_length);
	void NC_Road_Destroy(const unsigned short x, const unsigned short y, const unsigned char dir);
	void NC_ChangeDistribution(unsigned char * const distri_data);
	void NC_ChangeBuildOrder(const unsigned char order_type,const unsigned char * const order);
	void NC_SetBuildingSite(const unsigned short x, const unsigned short y, const unsigned char type);
	void NC_DestroyBuilding(const unsigned short x, const unsigned short y);
	void NC_ChangeTransport(const unsigned char * const transport_data);
	void NC_ChangeMilitary(const unsigned char * const military_data);
	void NC_ChangeTools(const unsigned char * const tools_data);
	void NC_CallGeologist(const unsigned short x, const unsigned short y);
	void NC_CallScout(const unsigned short x, const unsigned short y);
	void NC_Attack(const unsigned short x, const unsigned short y, const unsigned short soldiers_count,const bool strong_soldiers);
	void NC_SwitchPlayer(const unsigned char new_id);
	void NC_StopGold(const unsigned short x, const unsigned short y);
	void NC_StopProduction(const unsigned short x, const unsigned short y);
	void NC_ChangeInventorySetting(const unsigned short x, const unsigned short y,unsigned char category,unsigned char state,unsigned char type);
	void NC_ChangeAllInventorySettings(const unsigned short x, const unsigned short y,const unsigned char category,const unsigned char state);
	void NC_IncreaseReserve(const unsigned short x, const unsigned short y, const unsigned char rank);
	void NC_DecreaseReserve(const unsigned short x, const unsigned short y, const unsigned char rank);
	void NC_CheatArmageddon();
  void NC_DestroyAll();
	void NC_Surrender();

	void Command_SetFlag2(int x, int y, unsigned char player);
	void Command_Chat(const std::string& text, const ChatDestination cd );
	void Command_ToggleNation();
	void Command_ToggleTeam();
	void Command_ToggleColor();
	void Command_ToggleReady();


	/// Lädt ein Replay und startet dementsprechend das Spiel (0 = alles OK, alles andere entsprechende Fehler-ID!)
	unsigned StartReplay(const std::string &path, GameWorldViewer * &gwv);
	/// Replay-Geschwindigkeit erhöhen/verringern
	void IncreaseReplaySpeed() { if(replay_mode && framesinfo.gf_length>10) framesinfo.gf_length-=10; }
	void DecreaseReplaySpeed() { if(replay_mode && framesinfo.gf_length<1000) framesinfo.gf_length+=10; }
	void SetReplayPause(bool pause);
	void ToggleReplayPause() { SetReplayPause(!framesinfo.pause); }
	/// Schaltet FoW im Replaymodus ein/aus
	void ToggleReplayFOW() { replayinfo.all_visible = !replayinfo.all_visible; }
	/// Prüft, ob FoW im Replaymodus ausgeschalten ist
	bool IsReplayFOWDisabled() const { return replayinfo.all_visible; }
	/// Gibt Replay-Ende (GF) zurück
	unsigned GetLastReplayGF() const { return replayinfo.replay.last_gf; }

	/// Gibt Replay-Dateiname zurück
	const std::string& GetReplayFileName() const { return replayinfo.filename; }
	/// Wird ein Replay abgespielt?
	bool IsReplayModeOn() const { return replay_mode; }

	void SkipGF(unsigned int gf);

	/// Wechselt den aktuellen Spieler (nur zu Debugzwecken !!)
	void ChangePlayer(const unsigned char old_id,const unsigned char new_id);

	/// Wechselt den aktuellen Spieler im Replaymodus
	void ChangeReplayPlayer(const unsigned new_id);

	/// Laggt ein bestimmter Spieler gerade?
	bool IsLagging(const unsigned int id) { return players[id]->is_lagging; }

	/// Spiel pausiert?
	bool IsPaused() const { return framesinfo.pause; }

	/// Schreibt Header der Save-Datei
	unsigned WriteSaveHeader(const std::string& filename);

	/// Visuelle Einstellungen aus den richtigen ableiten
	void GetVisualSettings();

	/// Zum Log etwas hinzufügen
	void AddToGameLog(const char * const str);


private:

	void FillPlayerQueues(void);
	void HandleMessage(GameMessage *message);
	void HandleGameMessage(GameMessage *message);
	/// Versucht einen neuen GameFrame auszuführen, falls die Zeit dafür gekommen ist
	void ExecuteGameFrame(const bool skipping = false);
	void ExecuteGameFrame_Replay();
	void ExecuteGameFrame_Game();
	/// Filtert aus einem Network-Command-Paket alle Commands aus und führt sie aus, falls ein Spielerwechsel-Command
	/// dabei ist, füllt er die übergebenen IDs entsprechend aus
	void ExecuteAllNCs(unsigned char * data, unsigned char player,  unsigned char * player_switch_old_id,unsigned char * player_switch_new_id);
	/// Sendet ein NC-Paket ohne Befehle
	void SendNothingNC(int checksum = -1);


	//  Netzwerknachrichten
	void OnNMSDeadMsg(GameMessage *message);

	void OnNMSPing(GameMessage *message);

	void OnNMSPlayerId(GameMessage *message);
	void OnNMSPlayerList(GameMessage *message);
	void OnNMSPlayerNew(GameMessage *message);
	void OnNMSPlayerPing(GameMessage *message);
	void OnNMSPlayerToggleState(GameMessage *message);
	void OnNMSPlayerToggleNation(GameMessage *message);
	void OnNMSPlayerToggleTeam(GameMessage *message);
	void OnNMSPlayerToggleColor(GameMessage *message);
	void OnNMSPlayerReady(GameMessage *message);
	void OnNMSPlayerKicked(GameMessage *message);
	void OnNMSPlayerSwap(GameMessage *message);

	void OnNMSServerTyp(GameMessage *message);
	void OnNMSServerPassword(GameMessage *message);
	void OnNMSServerName(GameMessage *message);
	void OnNMSServerChat(GameMessage *message);
	void OnNMSServerStart(GameMessage *message);
	void OnNMSServerAsync(GameMessage *message);

	void OnNMSMapName(GameMessage *message);
	void OnNMSMapInfo(GameMessage *message);
	void OnNMSMapData(GameMessage *message);
	void OnNMSMapChecksum(GameMessage *message);

	void OnNMSGGSChange(GameMessage *message);

	void OnNMSNfcAnswer(GameMessage *message);
	void OnNMSNfcDone(GameMessage *message);
	void OnNMSNfcPause(GameMessage *message);

	/// Wird aufgerufen, wenn der Server gegangen ist (Verbindung verloren, ungültige Nachricht etc.)
	void ServerLost();


	// Replaymethoden

	/// Schreibt den Header der Replaydatei
	void WriteReplayHeader(const unsigned random_init);




public:

	/// Virtuelle Werte der Einstellungsfenster, die aber noch nicht wirksam sind, nur um die Verzögerungen zu
	/// verstecken
	struct VisualSettings
	{
		/// Verteilung
		unsigned char distribution[20];
		/// Art der Reihenfolge (0 = nach Auftraggebung, ansonsten nach build_order)
		unsigned char order_type;
		/// Baureihenfolge
		unsigned char build_order[31];
		/// Transport-Reihenfolge
		unsigned char transport_order[14];
		/// Militäreinstellungen (die vom Militärmenü)
		unsigned char military_settings[7];
		/// Werkzeugeinstellungen (in der Reihenfolge wie im Fenster!)
		unsigned char tools_settings[12];
	} visual_settings, default_settings;


private:

	/// Spielwelt
	GameWorld * gw;
	/// EventManager
	EventManager * em;
	/// Spieler
	GameClientPlayer ** players;
	/// Anzahl Spieler
	unsigned char player_count;
	/// Spieler-ID dieses Clients
	unsigned char playerid;
	/// Globale Spieleinstellungen
	GlobalGameSettings ggs;

	GameMessageQueue recv_queue, send_queue;
	Socket so;
	// Was soll das sein? oO
	unsigned int temp_ul;
	unsigned int temp_ui;


	ClientState state;

	class ClientConfig
	{
	public:
		ClientConfig() { Clear(); }
		void Clear();

		std::string server;
		std::string gamename;
		std::string password;
		std::string mapfile;
		std::string mapfilepath;
		unsigned char servertyp;
		unsigned short port;
		bool host;
	} clientconfig;

	class MapInfo
	{
	public:
		MapInfo() { Clear(); }
		void Clear();

		MapType map_type;
		unsigned partcount;
		unsigned ziplength;
		unsigned length;
		unsigned checksum;
		std::string title;
		unsigned char *zipdata;
		Savegame savegame;
	} mapinfo;

	class FramesInfo
	{
	public:
		FramesInfo() { Clear(); }
		void Clear();

		/// Aktueller GameFrame (der wievielte seit Beginn)
		unsigned nr;
		/// Länge der GameFrames in ms (= Geschwindigkeit des Spiels)
		unsigned gf_length;
		/// Länge der Network-Frames in gf(!)
		unsigned nwf_length;

		/// Zeit in ms seit dem letzten Frame
		unsigned frame_time;

		unsigned lasttime;
		unsigned lastmsgtime;
		unsigned pausetime;

		bool pause;
	} framesinfo;

	class RandCheckInfo
	{
	public:
		RandCheckInfo() { Clear(); }
		void Clear();

		int rand;
		int last_rand;
	} randcheckinfo;


	ClientInterface *ci;

	list<GameMessage*> nfc_queue;

	struct ReplayInfo
	{
	public:
		ReplayInfo() { Clear(); }
		void Clear();

		/// Replaydatei
		Replay replay;
		/// Replay asynchron (Meldung nur einmal ausgeben!)
		bool async;
		bool end;
		// Nächster Replay-Command-Zeitpunkt (in GF)
		unsigned next_gf;
		/// Replay-Dateiname
		std::string filename;
		/// Alles sichtbar (FoW deaktiviert)
		bool all_visible;
	} replayinfo;

	/// Replaymodus an oder aus?
	bool replay_mode;

	/// Spiel-Log für Asyncs
	FILE * game_log;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define GAMECLIENT GameClient::inst()

#endif
