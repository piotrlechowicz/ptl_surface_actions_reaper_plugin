//
//  show-only-tracks.hpp
//  reaper_ptl_surface_actions
//
//  Created by Piotr Lechowicz on 02/08/2021.
//  Copyright © 2021 My Company. All rights reserved.
//

#ifndef show_only_tracks_hpp
#define show_only_tracks_hpp

#include <stdio.h>

#include "utilfuncs.h"
#include "reaper_action_helper.h"
#include "reaper_plugin_functions.h"
#include <string>
#include <vector>

namespace surface_actions {

void registerActionsShowOnlySpecifiedTracks();

/**
 * Show only specified tracks
 *
 * Based on Lokasenna scripts
 * `show only specified tracks.lua`
 */

class ShowOnlySpecifiedTracks
{
public:	
	void execute();

protected:
	virtual std::string getUndoActionName() = 0;
	
	struct Settings
	{
		std::string searchName; 	// search tracks with given name
		bool matchMultiple = true;
		bool matchOnlyTop = true;
		bool showChildren = true;
		bool showParents = true;
		bool showSiblings = false;
		
		bool mcp = true;
		bool tcp = true;
	};
	
	Settings settings;
	
private:
	int bufferSize = 4096;
	
	/** get boolean vector defining which tracks should be visible, and which should not */
	virtual std::vector<bool> getTracksVisibility();
	
	/** change tracks visibility in reaper*/
	void applyTracksVisibilityInReaper(const std::vector<bool>& tracksVisibility);
	
	void selectFirstVisibleMCP();
	
	void applyLogicalOrToVectorLHS(std::vector<bool>& lhs, const std::vector<bool>& rhs);
	
	/* return parents of matched tracks (as list of bools) */
	void getParents(const std::vector<bool>& matchedTracks, std::vector<bool>& resultParents);
	/* return recursive parents of given track */
	void getRecursiveParents(MediaTrack* track, std::vector<MediaTrack*>& result);
	/* return children of matched tracks */
	void getChildren(const std::vector<bool>& matchedTracks, std::vector<bool>& resultChildren);
	void getTopLevelTracks(std::vector<bool>& resultTopLevelTracks);
	void getSiblings(const std::vector<bool>& matchedTracks, std::vector<bool>& resultSiblings);
};

/* Show only tracks implementations */

class ShowAllTracks : public ShowOnlySpecifiedTracks {
public:
	inline std::vector<bool> getTracksVisibility()
	{
		return std::vector<bool>(CountTracks(0), true);
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show all tracks"; }
};

class ShowOnlyTopLevelTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyTopLevelTracks() {
		settings.searchName = "";
		settings.matchOnlyTop = true;
		settings.showChildren = false;
		settings.showSiblings = false;
		settings.showParents = false;
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show top level tracks"; }
};

class ShowOnlyBusTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyBusTracks() {
		settings.searchName = "BUS";
		settings.showChildren = false;
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Bus tracks (pattern=\"BUS\")"; }
};

class ShowOnlyFXBusTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyFXBusTracks() {
		settings.searchName = "FX";
		settings.showChildren = false;
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show FX tracks (pattern=\"FX\")"; }
};

class ShowOnlyVocalTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyVocalTracks() {
		settings.searchName = "Vocal";
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Vocal tracks (pattern=\"Vocal\")"; }
};



class ShowOnlyDrumsTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyDrumsTracks() {
		settings.searchName = "Drum";
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Drums tracks (pattern=\"Drum\")"; }
};

class ShowOnlyBassTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyBassTracks() {
		settings.searchName = "Bass";
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Bass tracks (pattern=\"Bass\")"; }
};

class ShowOnlyGuitarTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyGuitarTracks() {
		settings.searchName = "Guitar";
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Guitar tracks (pattern=\"Guitar\")"; }
};

class ShowOnlyPianoTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlyPianoTracks() {
		settings.searchName = "Piano";
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Piano tracks (pattern=\"Piano\")"; }
};

class ShowOnlySynthTracks : public ShowOnlySpecifiedTracks {
public:
	inline ShowOnlySynthTracks() {
		settings.searchName = "Synth";
	}

	inline std::string getUndoActionName() { return "Show only specified tracks - Show Synth tracks (pattern=\"Synth\")"; }
};

}

#endif /* show_only_tracks_hpp */
