# HTTP TODO

- Integrate `Context::checkTimeout()` into the connection event-loop sweep so
  completely silent clients expire without another read event.
