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
