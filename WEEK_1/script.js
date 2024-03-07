document.getElementById("feedbackForm").addEventListener("submit", function(event) {
    event.preventDefault(); 

   
    var feedbackMessage = document.getElementById("feedbackMessage");
    feedbackMessage.style.display = "block";

   
    document.getElementById("feedbackForm").reset();
});
