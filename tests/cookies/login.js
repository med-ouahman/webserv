
const formDataDiv = document.getElementById("cookies");
const form = document.getElementById("form");
const username = document.getElementById("username");
const submitButton = document.getElementById("submit");
const errorUsername = document.getElementById("error-username");
const userDataDiv = document.getElementById("user-data");
const dataDiv = document.querySelector("#user-data #data");
const gobackBtn = document.getElementById("go-back");
const logoutBtn = document.getElementById("logout");

let defaultDisplay = "block";

let inForm = true;
let erroUserHidden = true;

if (errorUsername.style.display.length > 0) {
    defaultDisplay = errorUsername.style.display;
}

hideErrorUsername();

let processing = false;

const formURL = form.action;

function showErrorUsername() {
    if (!erroUserHidden) return;
    errorUsername.style.display = defaultDisplay;
    erroUserHidden = false;
}

function hideErrorUsername() {
    if (erroUserHidden) return;
    defaultDisplay = errorUsername.style.display;
    errorUsername.style.display = "none";
    erroUserHidden = true;
}

submitButton.addEventListener("click", (event) => {
    event.preventDefault();

    if (processing) return;

    if (username.value === "") {
        showErrorUsername();
        return;
    }

    processing = true;

    const body = "username="+username.value;

    fetch(form.action, {
        method: form.method,
        body,
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        }
    })
    .then((response) => {
        
        if (!response.ok) {
            renderError(response);
            return;
        }

        return response.text();
    })
    .then((data) => {
        displayUserData(data);
    })
    .catch((error) => {
        console.log("Error:", error);
    })
    .finally(() => {
        processing = false;
        username.value = "";
    });

});

username.addEventListener("input", () => {
    hideErrorUsername();
});


function displayUserData(data) {
    inForm = false;
    formDataDiv.style.display = "none";
    userDataDiv.style.display = "flex";
   
    dataDiv.innerHTML = data;
}

function gobackToForm() {
    if (inForm) return;

    formDataDiv.style.display = "flex";
    userDataDiv.style.display = "none";
}

function renderError(response) {
    document.body.innerHTML = `<h1> ${response.status} ${response.statusText} </h1>`;
}

function renderMessage(message) {
    document.body.innerHTML = message;
}


gobackBtn.addEventListener("click", gobackToForm);

logoutBtn.addEventListener("click", logout);

function logout() {
    
    if (processing) return;

    fetch("/logout", {
        method: "DELETE",
    })

    .then((response) => {
        
        if (!response.ok) {
            renderError(response);
            return;
        }

        return response.text();
    })
    .then((data) => {
        renderMessage(data);
    })
    .catch((error) => {
        console.log("Error:", error);
    })
    .finally(() => {
        processing = false;
    });
}