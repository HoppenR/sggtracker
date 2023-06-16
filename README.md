# sggtracker
set callbacks like print/log for messages in sgg

## Usage
```bash
[handler_type] [track_type] [query_string]
```
## Example commands
```bash
print emote billyWeird
log mention HoppenR
log match hop
```

## Capabilities
### Handlers
- [X] `print`: print matches to stdout
- [ ] `log`: log matches to file

### Tracks
- [X] `name`: (message author)
- [X] `match`: (matches substring in the message content)
- [X] `emote`: (valid emote in the message)
- [X] `mention`: (valid tag in the message)
