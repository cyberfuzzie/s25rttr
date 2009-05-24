// $Id: Msgbox.h 4933 2009-05-24 12:29:23Z OLiver $
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
#ifndef MSGBOX_H_INCLUDED
#define MSGBOX_H_INCLUDED

#pragma once

enum MsgboxButton
{
	MSB_OK,
	MSB_OKCANCEL,
	MSB_YESNO,
	MSB_YESNOCANCEL
};

enum MsgboxIcon
{
	MSB_QUESTIONGREEN =	72,
	MSB_EXCLAMATIONGREEN,
	MSB_QUESTIONRED,
	MSB_EXCLAMATIONRED
};

enum MsgboxResult
{
	MSR_OK = 0,
	MSR_CANCEL,
	MSR_YES,
	MSR_NO,
	MSR_NOTHING = 0xFFFFFFFF
};

#endif //!MSGBOX_H_INCLUDED
