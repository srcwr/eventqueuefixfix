[eventqueuefix](https://github.com/hermansimensen/eventqueue-fix) is pretty cool but doesn't work on Windows after the 2025-02-18 update.

eventqueuefix hooks `CEventQueue::AddEvent()` to do its magic. On Windows, the hook isn't always called because the function is also inlined inside `CBaseEntityOutput::FireOutput()`.

This extension hooks `CBaseEntityOutput::FireOutput()` to make it call `AddEvent()` so it reaches eventqueuefix.

"Why not just hook FireOutput in eventqueuefix?":
- Because it's a little fucky to replicate in Sourcepawn and wouldn't work in x64 anyway, so an extension will be more future-proof.
