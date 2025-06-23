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
    </head>
    <style>
    body {
  overflow-y: hidden;
   overflow-x: hidden;
        }
    </style>

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
    <h1 class="display-2 mb-5">Server status <span id="stats"> </span></h1>
    <button class="btn btn-primary btn-lg px-5 py-3" onclick="klik()">Wake me up</button>
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
     
  function klik() {
    fetch('/klik')
      .then(resp => console.log("Odoslané na ESP"))
      .catch(err => console.error("Chyba:", err));

       alert("diiik jak ma kliklo");
  }


 async function updateStatus() {
    try {
      const response = await fetch("/online");
      const data = await response.json();
      ServerStatus(data.online);
    } catch (e) {
      console.error("Chyba pri fetchovaní /online:", e);
      ServerStatus(false); // záložný fallback
    }
  }

  // spusti hneď a potom opakuj každých 5 sekúnd:
  updateStatus();
  setInterval(updateStatus, 5000);

     function ServerStatus(status){

            if(status){ // if true then ->
                document.getElementById("stats").innerText="on";
                document.getElementById("stats").style.color="green";
            }
            else{
                document.getElementById("stats").innerText="off";
                document.getElementById("stats").style.color="red";

            }
}
        </script>
    </body>
</html>
)rawliteral";