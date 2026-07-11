
# Context State Refactor

- Add an HTTP timer object that tracks elapsed time and progress for request-line parsing, header parsing, body parsing, CGI execution, and response writing.

- Make timeout checks independent from event-loop iteration boundaries.
- Replace the `REQUEST_LINE` and `HEADERS` Context states with one `PARSING` state.
- Add a parser-local phase for request-line and header parsing because they still use different parsing rules, size limits, and timeouts.
- Remove the `ROUTING` Context state because routing is a one-step operation with no progress that must survive between calls.
- Store the routing decision as optional data so its absence indicates that routing has not run yet.
- After parsing completes, enter `PROCESSING`; `process()` should route when no decision exists and continue immediately into body or handler work when possible.
- Keep Context states only for work that can remain unfinished across calls: `PARSING`, `PROCESSING`, `CGI_RUNNING`, `DONE`, and `ERROR`.
- Keep `ContextAction` responsible for telling the connection layer whether HTTP needs reading, writing, internal work, or closure.
- Make `responseReady()` set the Context state to `DONE` and its action to `AC_WRITE`.
