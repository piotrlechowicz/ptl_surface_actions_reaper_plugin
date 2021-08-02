//
//  show-only-tracks.cpp
//  reaper_ptl_surface_actions
//
//  Created by Piotr Lechowicz on 02/08/2021.
//  Copyright © 2021 My Company. All rights reserved.
//

#include "show-only-tracks.hpp"
#include "surface-actions-utils.hpp"

namespace surface_actions {

void registerActionsShowOnlySpecifiedTracks()
{
	std::string namePrefix = "PTL Show Tracks: ";
	std::string idPrefix = "ptl_show_tracks_";
	add_action(namePrefix + "Show all tracks", idPrefix + "show_all_tracks", CannotToggle, [](action_entry&) {
			ShowAllTracks action;
			action.execute();
		});
	add_action(namePrefix + "Show all top level tracks", idPrefix + "show_all_top_level_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyTopLevelTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Bus tracks (pattern=\"BUS\")", idPrefix + "show_only_bus_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyBusTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show FX Bus tracks (pattern=\"FX\")", idPrefix + "show_only_fx_bus_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyFXBusTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Vocal tracks (pattern=\"Vocal\")", idPrefix + "show_only_vocal_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyVocalTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Drums tracks (pattern=\"Drum\")", idPrefix + "show_only_drums_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyDrumsTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Bass tracks (pattern=\"Bass\")", idPrefix + "show_only_bass_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyBassTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Guitar tracks (pattern=\"Guitar\")", idPrefix + "show_only_guitar_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyGuitarTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Piano tracks (pattern=\"Piano\")", idPrefix + "show_only_piano_tracks", CannotToggle, [](action_entry&) {
		ShowOnlyPianoTracks action;
		action.execute();
	});
	add_action(namePrefix + "Show Synth tracks (pattern=\"Synth\")", idPrefix + "show_only_synth_tracks", CannotToggle, [](action_entry&) {
		ShowOnlySynthTracks action;
		action.execute();
	});
}


//---------------------
// ShowOnlySpecifiedTracks
//---------------------
// public members
//---------------------

void ShowOnlySpecifiedTracks::execute()
{
	Undo_BeginBlock();
	PreventUIRefresh(1);
	
	std::vector<bool> tracksVisibility = this->getTracksVisibility();
	this->applyTracksVisibilityInReaper(tracksVisibility);
	
	PreventUIRefresh(-1);
	Undo_EndBlock(this->getUndoActionName().c_str(), -1);
	
	TrackList_AdjustWindows(false);
	UpdateArrange();
}

//---------------------
// protected members
//---------------------



//----------------------
// private members
//----------------------

std::vector<bool> ShowOnlySpecifiedTracks::getTracksVisibility()
{
	int nrOfTracks = CountTracks(0);
	
	std::vector<bool> tracksVisibility(nrOfTracks, false);
	
	// abort if nothing needed to be done
	if (!settings.tcp && !settings.mcp) return tracksVisibility;
	
	// list of tracks that matches the query
	std::vector<bool> matchedTracks(nrOfTracks, false);
	
	for (int trackIdx = 0; trackIdx < nrOfTracks; trackIdx++)
	{
		MediaTrack* track = GetTrack(0, trackIdx);
		std::shared_ptr<char> trackName(new char[bufferSize]);
		GetTrackName(track, trackName.get(), bufferSize);
		
		bool isChild = GetTrackDepth(track) > 0;
		
		if (!(isChild && settings.matchOnlyTop)) // if is a child and looking for only top level then do not cosider the track
		{
			if (stringFuzzyMatch(std::string(trackName.get()), settings.searchName))
			{
				matchedTracks.at(trackIdx) = true;
				if (!settings.matchMultiple) break; // if not looking for more than one track then exit
			}
		}
	}
	
	if (matchedTracks.size() == 0) return tracksVisibility; // nothing matches the query
	
	tracksVisibility.assign(matchedTracks.begin(), matchedTracks.end());
	
	if (settings.showParents == true)
	{
		std::vector<bool> parents(nrOfTracks, false);
		getParents(matchedTracks, parents);
		applyLogicalOrToVectorLHS(tracksVisibility, parents);
	}
	if (settings.showChildren == true)
	{
		std::vector<bool> children(nrOfTracks, false);
		getChildren(matchedTracks, children);
		applyLogicalOrToVectorLHS(tracksVisibility, children);
	}
	if (settings.showSiblings == true)
	{
		std::vector<bool> siblings(nrOfTracks, false);
		getSiblings(matchedTracks, siblings);
		applyLogicalOrToVectorLHS(tracksVisibility, siblings);
	}
	return tracksVisibility;
}

void ShowOnlySpecifiedTracks::applyTracksVisibilityInReaper(const std::vector<bool>& tracksVisibility)
{
	for (int i = 0; i < CountTracks(0); i++)
	{
		MediaTrack* track = GetTrack(0, i);
		if (this->settings.tcp)
			SetMediaTrackInfo_Value(track, "B_SHOWINTCP", tracksVisibility.at(i) ? 1 : 0);
		if (this->settings.mcp)
			SetMediaTrackInfo_Value(track, "B_SHOWINMIXER", tracksVisibility.at(i) ? 1 : 0);
	}
	if (this->settings.mcp)
		selectFirstVisibleMCP();
}

void ShowOnlySpecifiedTracks::selectFirstVisibleMCP()
{
	for (int i = 0; i < CountTracks(0); i++)
	{
		MediaTrack* track = GetTrack(0, i);
		if (IsTrackVisible(track, true))
		{
			SetOnlyTrackSelected(track);
			break;
		}
	}
}

void ShowOnlySpecifiedTracks::applyLogicalOrToVectorLHS(std::vector<bool>& lhs, const std::vector<bool>& rhs)
{
	for (int i = 0; i < lhs.size(); i++)
	{
		lhs.at(i) = lhs.at(i) || rhs.at(i);
	}
}

void ShowOnlySpecifiedTracks::getParents(const std::vector<bool> &matchedTracks, std::vector<bool> &resultParents)
{
	for (int trackIdx = 0; trackIdx < matchedTracks.size(); trackIdx++)
	{
		if (matchedTracks.at(trackIdx))
		{
			MediaTrack* track = GetTrack(0, trackIdx);
			if (GetTrackDepth(track) > 0)
			{
				std::vector<MediaTrack*> recursiveResult;
				this->getRecursiveParents(GetParentTrack(track), recursiveResult);
				for (MediaTrack* track : recursiveResult)
					// track number is 1-based
					resultParents.at((int)(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER") - 1)) = true;
			}
		}
	}
}

void ShowOnlySpecifiedTracks::getRecursiveParents(MediaTrack *track, std::vector<MediaTrack *> &result)
{
	if (GetTrackDepth(track) == 0)
	{
		result.push_back(track);
	}
	else
	{
		this->getRecursiveParents(GetParentTrack(track), result);
		result.push_back(track);
	}
}

void ShowOnlySpecifiedTracks::getChildren(const std::vector<bool> &matchedTracks, std::vector<bool> &resultChildren)
{
	for (int trackIdx = 0; trackIdx < matchedTracks.size(); trackIdx++)
	{
		if (matchedTracks.at(trackIdx))
		{
			MediaTrack* track = GetTrack(0, trackIdx);
			
			// children tracks only can have higher indices
			for (int i = trackIdx + 1; i < matchedTracks.size(); i++)
			{
				std::vector<MediaTrack*> recursiveResult;
				this->getRecursiveParents(GetTrack(0, i), recursiveResult);
				// if one of the parents is equal to current track
				if (std::find(recursiveResult.begin(), recursiveResult.end(), track) != recursiveResult.end())
				{
					resultChildren.at(i) = true;
				}
				if (!resultChildren.at(i))
				{
					// if track is not a children we can skip the loop - all of the next tracks
					// will not be a children of current track.
					// Thus, we should continue from other track in matchedTracks list
					break;
				}
			}
		}
	}
}

void ShowOnlySpecifiedTracks::getTopLevelTracks(std::vector<bool> &resultTopLevelTracks)
{
	for (int i = 0; i < resultTopLevelTracks.size(); i++)
		if (GetTrackDepth(GetTrack(0, i)) == 0)
			resultTopLevelTracks.at(i) = true;
}

void ShowOnlySpecifiedTracks::getSiblings(const std::vector<bool> &matchedTracks, std::vector<bool> &resultSiblings)
{
	for (int trackIdx = 0; trackIdx < matchedTracks.size(); trackIdx++)
	{
		if (matchedTracks.at(trackIdx))
		{
			MediaTrack* track = GetTrack(0, trackIdx);
			int siblingDepth = GetTrackDepth(track);
			if (siblingDepth > 0)
			{
				MediaTrack* parent = GetParentTrack(track);
				
				std::vector<bool> parentIdVector(matchedTracks.size(), false);
				std::vector<bool> childrenResult(matchedTracks.size(), false);
				// track ids are 1-based in Reaper
				parentIdVector.at(GetMediaTrackInfo_Value(parent, "IP_TRACKNUMBER") - 1) = true;
				getChildren(parentIdVector, childrenResult);
				
				for (int childIdx = 0; childIdx < childrenResult.size(); childIdx++)
					if (childrenResult.at(childIdx))
						if (GetTrackDepth(GetTrack(0, childIdx)) == siblingDepth)
							resultSiblings.at(childIdx) = true;
			}
			else
			{
				// find all top level tracks
				getTopLevelTracks(resultSiblings);
			}
		}
	}
}

}
