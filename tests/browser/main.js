function requestToHttp(request) {
    let http = `${request.method} ${request.url} ${request.version}\r\n`;

    for (const [name, value] of Object.entries(request.headers)) {
        http += `${name}: ${value}\r\n`;
    }

    http += "\r\n";

    if (request.body)
        http += request.body;

    return http;
}

async function sendRequest(request) {

    const d = await fetch(request.url, {
        method: request.method,
        headers: request.headers,
        body: request.body
    });
    
    const text = await d.text();

    return text;
}

function createRequest(reqEl) {
    const addHeader = reqEl.addHeaderBtn;

    const request = {
        method: reqEl.method.value,
        url: reqEl.url.value,
        version: reqEl.version.value,
        headers: [],
        body: reqEl.body.value
    };

    addHeader.addEventListener("click", () => {
        request.headers.push(reqEl.headers.value);
        reqEl.headers.value = "";
    });
    
}


const requestElements = {
    method: document.getElementById("method"),
    url: document.getElementById("url"),
    version: document.getElementById("version"),
    headers: document.getElementById("headers"),
    addHeaderBtn: document.getElementById("add-header"),
    body: document.getElementById("body"),
    sendBtn: document.getElementById("send-request")
}

const request =  {
    method: requestElements.method.value,
    url: requestElements.url.value,
    version: requestElements.version.value,
    headers: [],
    body: requestElements.body.value
};


requestElements.sendBtn.addEventListener("click", () => {
    request.method = requestElements.method.value,
    request.url = requestElements.url.value,
    request.version = requestElements.version.value,
    request.body = requestElements.body.value
    async function wait() {

        const response = await sendRequest(request);
        
        document.getElementById("response-content").innerHTML = response;
    }
    wait();
});


const seeRequestBtn = document.getElementById("show-request");
const hideRequestBtn = document.getElementById("hide-request");
const requestContainer = document.querySelector(".show-request-toggle");

seeRequestBtn.addEventListener('click', () => {
    seeRequestBtn.classList.add("hidden");
    requestContainer.classList.remove("hidden");
});

hideRequestBtn.addEventListener("click", () => {
    requestContainer.classList.add("hidden");
    seeRequestBtn.classList.remove("hidden");
});