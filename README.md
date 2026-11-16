# gc-reforger-units
[GC Units](https://reforger.armaplatform.com/workshop/6156F2F771D5D73D-GlobalConflictsUnits) is a Reforger mod containing shared units used by Global Conflicts Arma community.

To contribute a custom faction, reference the [custom faction creation guide](https://github.com/Global-Conflicts-ArmA/gc-reforger-missions/wiki/Custom-faction-creation)

# Content

## GC Lobby Dev

Units Dev uses GC Lobby Dev group metadata. In the group's **GC Lobby** category,
set the call sign, company/platoon/squad indices and **Description**. Description
identifies the group's purpose beneath its larger call sign in the lobby.

Group **Playable** controls player slots for every member. `_P` group prefabs
remain playable by default; explicitly AI variants remain non-playable. Mission
makers can change this on placed groups. The USSR AGS squad exposes all nine
members as player slots. A standalone `_P` character does not create a lobby slot;
use it as a member of a playable group.

Legacy custom group names now supply Description, and legacy callsign indices use
the new group fields. Custom character role labels use native editable-character
UI info. Legacy PS components and old Lobby character wrappers have been removed
while preserving unit loadouts and local prefab identities.

Validation with the installed mod pack found unresolved legacy US desert parent
references: 49 character prefabs lack usable role UI info, mainly deprecated DBDU
units and some Green Berets/1998 desert variants. These references predate this
migration and still need their original content repaired or replaced before use.

## BLUFOR
- CDF Army
- RHS USAF
- UK Army 
- US Army
- Canadian Army

## OPFOR
- GC Insurgents
- GC Separatist
- GC IRA
- RHS AFRF
- USSR Army

## INDFOR
- FIA
- ION
  - Uniformed Services
  - VIP Protection

## Dependency ownership

Units owns unit/loadout content and equipment changes: weapons, ammunition,
mortars, vehicles, boats, sights, supporting effects and their overrides. The
former Core equipment changes are now here, retaining resource GUIDs. Shared
ballistic tables remain here with their ammunition.

Units depends on Core and Lobby Dev, its equipment providers, and GC's selected
shared gameplay baseline (including Adult Mortars and GC Suppression). Core
requires only the systems its own changes directly use. Units does not depend
on the GC Workshop collection or custom terrain packs. Legacy TILW crew
configurations still require TilW explicitly; this does not add that dependency
to GC Lobby or Core. Update Core, Units and Missions Dev together after the split.

GC Workshop loads the complete collection; it is not a dependency of this addon.
This is still a shared package: loading Units loads its declared equipment library,
not just the particular equipment placed in a mission.
