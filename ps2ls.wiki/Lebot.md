#Using lebot


## Description

lebot is the patch content analysis bot that is in `#REside`

## Commands

All commands are prefixed with `.`

Patches are in the form :
* GUX where X is the game update.  Example: `GU3`
* GUXhfY where X is the game update and Y is the patch number.  Example: `GU3hf2` is the patch released on 3-1-2012

#### General
* `ping` Returns a pong
* `lastupdate` returns the date of the last patch for PS2 Live and Test

#### Census queries
* `stats <player>` queries census for the player's stats
* `lastquery` returns the last census query that was run

#### Revision DB queries
* `find <string>` Finds all files with `string` in the name
* `lookup <filename>` Looks up the revision history for `filename`
* `report_revinfo <patch>` Reports general information related to `patch`
* `report_if <patch>` Reports the Interesting Files for revision `patch`.  Interesting files are files that contain a string noted in the interesting files tags.
* `report_chng <patch>` Reports all changed files in the revision.

#### Messaging
* `register <password>` registers an account with lebot
* `togglesysmessages` enables lebot to message you when an important event occurs (currently when Planetside 2 is updated)

