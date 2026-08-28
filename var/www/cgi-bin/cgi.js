#!/usr/bin/env node

const fs = require("fs");


let body = "";

process.stdin.setEncoding("utf8");

process.stdin.on("data", chunk => {
    body += chunk;
});

process.stdin.on("end", () => {
    console.log("Content-Type: text/plain");
    console.log("");

    console.log("Hello from Node.js CGI!");
    console.log("");

    console.log("Method:", process.env.REQUEST_METHOD || "");
    console.log("Query:", process.env.QUERY_STRING || "");
    console.log("Content-Type:", process.env.CONTENT_TYPE || "");
    console.log("Content-Length:", process.env.CONTENT_LENGTH || "");
    console.log("");

    console.log("Body:");
    console.log(body);
});
