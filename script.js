function analyzeCode() {
    const code = document.getElementById("code").value;

    fetch("http://127.0.0.1:5000/analyze", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: "code=" + encodeURIComponent(code)
    })
    .then(res => res.json())
    .then(data => {
        const tokenList = document.getElementById("tokenList");
        const errorList = document.getElementById("errorList");
        tokenList.innerHTML = "";
        errorList.innerHTML = "";

        data.tokens.forEach(token => {
            const li = document.createElement("li");
            li.textContent = `Token(${token[0]}, ${token[1]}, line=${token[2]})`;
            tokenList.appendChild(li);
        });

        data.errors.forEach(err => {
            const li = document.createElement("li");
            li.textContent = err;
            errorList.appendChild(li);
        });
    });
}
