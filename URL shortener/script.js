
document.getElementById('short-btn').addEventListener('click', () => {
    const longURL = document.getElementById('longURL').value.trim();

    if (!longURL) {
        document.getElementById('shortUrl').value = "Please enter a valid URL.";
        return;
    }

    // Fetch the shortened URL from TinyURL API
    const apiURL = `https://tinyurl.com/api-create.php?url=${encodeURIComponent(longURL)}`;

    fetch(apiURL)
        .then(response => response.text())
        .then(shortUrl => {
            document.getElementById('shortUrl').value = shortUrl;
        })
        .catch(error => {
            console.error("Error shortening URL:", error);
            document.getElementById('shortUrl').value = "Error: Unable to shorten URL.";
        });
});

document.getElementById('reload-btn').addEventListener('click', () => {
    document.getElementById('longURL').value = "";
    document.getElementById('shortUrl').value = "";
});
