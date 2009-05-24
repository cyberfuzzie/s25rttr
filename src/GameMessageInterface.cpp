// $Id: GameMessageInterface.cpp 4933 2009-05-24 12:29:23Z OLiver $
//
// Copyright (c) 2005-2008 Settlers Freaks (sf-team at siedler25.org)
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "GameMessageInterface.h"

void GameMessageInterface::OnNMSPing(const GameMessage_Ping& msg) {}
void GameMessageInterface::OnNMSPong(const GameMessage_Pong& msg) {}

void GameMessageInterface::OnNMSServerType(const GameMessage_Server_Type& msg) {}
void GameMessageInterface::OnNMSServerTypeOK(const GameMessage_Server_TypeOK& msg) {}
void GameMessageInterface::OnNMSServerPassword(const GameMessage_Server_Password& msg) {}
void GameMessageInterface::OnNMSServerName(const GameMessage_Server_Name& msg) {}
void GameMessageInterface::OnNMSServerStart(const GameMessage_Server_Start& msg) {}
void GameMessageInterface::OnNMSServerChat(const GameMessage_Server_Chat& msg) {}
void GameMessageInterface::OnNMSServerAsync(const GameMessage_Server_Async& msg) {}

void GameMessageInterface::OnNMSPlayerId(const GameMessage_Player_Id& msg) {}
void GameMessageInterface::OnNMSPlayerName(const GameMessage_Player_Name& msg) {}
void GameMessageInterface::OnNMSPlayerList(const GameMessage_Player_List& msg) {}
void GameMessageInterface::OnNMSPlayerToggleState(const GameMessage_Player_Toggle_State& msg) {}
void GameMessageInterface::OnNMSPlayerToggleNation(const GameMessage_Player_Toggle_Nation& msg) {}
void GameMessageInterface::OnNMSPlayerToggleTeam(const GameMessage_Player_Toggle_Team& msg) {}
void GameMessageInterface::OnNMSPlayerToggleColor(const GameMessage_Player_Toggle_Color& msg) {}
void GameMessageInterface::OnNMSPlayerKicked(const GameMessage_Player_Kicked& msg) {}
void GameMessageInterface::OnNMSPlayerPing(const GameMessage_Player_Ping& msg) {}
void GameMessageInterface::OnNMSPlayerNew(const GameMessage_Player_New& msg) {}
void GameMessageInterface::OnNMSPlayerReady(const GameMessage_Player_Ready& msg) {}
void GameMessageInterface::OnNMSPlayerSwap(const GameMessage_Player_Swap& msg) {}

void GameMessageInterface::OnNMSMapInfo(const GameMessage_Map_Info& msg) {}
void GameMessageInterface::OnNMSMapData(const GameMessage_Map_Data& msg) {}
void GameMessageInterface::OnNMSMapChecksum(const GameMessage_Map_Checksum& msg) {}
void GameMessageInterface::OnNMSMapChecksumOK(const GameMessage_Map_ChecksumOK& msg) {}


void GameMessageInterface::OnNMSPause(const GameMessage_Pause& msg) {}
void GameMessageInterface::OnNMSServerDone(const GameMessage_Server_NWFDone& msg) {}
void GameMessageInterface::OnNMSGameCommand(const GameMessage_GameCommand& msg) {}

void GameMessageInterface::OnNMSGGSChange(const GameMessage_GGSChange& msg) {}
