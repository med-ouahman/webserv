
Open questions / assumptions:

- I did not flag parser copies like `getChunk(..., std::string&)` as issues, because they look like a deliberate clarity tradeoff.

Summary:

- Parser logic looks materially better after the `processed` fix.
- The two real behavioral gaps still standing are timeout handling for body parsing and location-scoped body-size enforcement.
- After that, most of what stands out is debug residue and stale declarations rather than core parser breakage.
