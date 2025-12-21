const char* htmlPage PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
    <head>
        <title>WoWko</title>
        <link rel="icon" type="image/x-icon" href="https://www.iconpacks.net/icons/2/free-user-icon-3296-thumb.png">
        <meta charset="utf-8" />
        <meta
            name="viewport"
            content="width=device-width, initial-scale=1, shrink-to-fit=no"
        />
        <link
            href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css"
            rel="stylesheet"
            integrity="sha384-T3c6CoIi6uLrA9TneNEoa7RxnatzjcDSCmG1MXxSR1GAsXEV/Dwwykc2MPK8M2HN"
            crossorigin="anonymous"
        />
        <style>
            body {
                overflow-y: hidden;
                overflow-x: hidden;
            }
        </style>
    </head>

    <body>
        <header class="d-block d-sm-none">
           <nav class="navbar justify-content-center navbar-light bg-primary ">
              <span class="navbar-brand text-light h1 fs-1">Server Status</span>
           </nav>
        </header>

        <main class="d-none d-sm-flex justify-content-center align-items-center vh-100 bg-light">
          <h1 class="text-danger text-center">vaše zariadenie nie je podporované</h1>
        </main>

        <main class="d-block d-sm-none vh-100 d-flex justify-content-center align-items-center bg-white text-center">
          <div>
           <p id="startmsg" style="opacity: 0; transition: opacity 0.5s;">odhadovaný čas spustenia: <span id="timer"></span></p>
            <h1 class="display-2 mb-5">Server status <span id="stats">...</span></h1>
            <button class="btn btn-primary btn-lg px-5 py-3" id="disabler" onclick="klik()">Wake me up</button>
          </div>
        </main>

        <footer>
        </footer>

        <script
            src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.11.8/dist/umd/popper.min.js"
            integrity="sha384-I7E8VVD/ismYTF4hNIPjVp/Zjvgyol6VFvRkX/vR+Vc4jQkC+hVqc2pM8ODewa9r"
            crossorigin="anonymous"
        ></script>
        <script
            src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/js/bootstrap.min.js"
            integrity="sha384-BBtl+eGJRgqQAUMxJ7pMwbEyER4l1g+O15P+16Ep7Q9Q+zqX6gSbd85u4mG4QzX+"
            crossorigin="anonymous"
        ></script>

        <script>
            const serverStatusLabel = document.getElementById("stats");
            const disabledButton = document.getElementById("disabler");
            const startupmsg = document.getElementById("startmsg");
            const timerLabel = document.getElementById("timer");

            let seconds = 0; 
            let localTimer = null; 
            let serverOnline = false;

            function klik() {
                if (serverOnline || seconds > 0) return; 

                fetch('/klik')
                    .then(resp => {
                        console.log("Príkaz odoslaný");
                        setTimeout(updateStatus, 500); 
                    })
                    .catch(err => console.error("Chyba:", err));
                    
                disabledButton.disabled = true;
                alert("Požiadavka odoslaná. Čakám na potvrdenie servera...");
            }

            async function updateStatus() {
                try {
                    const response = await fetch("/online");
                    const data = await response.json();
                    processServerData(data);
                } catch (e) {
                    console.error("Chyba pri fetchovaní /online:", e);
                }
            }

            function processServerData(data) {
                const isOnline = data.online;
                const remainingSeconds = data.remaining; 

                if (isOnline) {
                    serverStatusLabel.innerText = "on";
                    serverStatusLabel.style.color = "green";
                    disabledButton.disabled = true;
                    
                    startupmsg.style.opacity = 0;
                    timerLabel.textContent = "";
                    seconds = 0;
                } else {
                    serverStatusLabel.innerText = "off";
                    serverStatusLabel.style.color = "red";
                    
                    if (remainingSeconds > 0) {
                         disabledButton.disabled = true;
                         startupmsg.style.opacity = 1; 
                         
                         if (Math.abs(seconds - remainingSeconds) > 2) {
                             seconds = remainingSeconds;
                         }
                         
                         if (!localTimer) startLocalLoop();
                         
                    } else {
                         disabledButton.disabled = false;
                         startupmsg.style.opacity = 0;
                         timerLabel.textContent = "";
                         stopLocalLoop();
                    }
                }
            }

            function startLocalLoop() {
                if (localTimer) return; 
                
                localTimer = setInterval(() => {
                    if (seconds > 0) {
                        seconds--;
                        renderTimer(seconds);
                    } else {
                        stopLocalLoop();
                    }
                }, 1000);
            }

            function stopLocalLoop() {
                if (localTimer) {
                    clearInterval(localTimer);
                    localTimer = null;
                }
            }

            function renderTimer(secLeft) {
                if (secLeft <= 0) {
                    timerLabel.textContent = "";
                    return;
                }
                const min = Math.floor(secLeft / 60);
                const sec = secLeft % 60;
                timerLabel.textContent = `${min}:${sec.toString().padStart(2, "0")}`;
            }

            // Spustenie
            updateStatus();
            setInterval(updateStatus, 5000); 
        </script>
    </body>
</html>
)rawliteral";