window.onload = start;

function start() {
  let brand = document.querySelector(".header_left");
  brand.addEventListener("click", () => {
    window.location.href = "https://zheng-bote.github.io/cake-planner-backend/";
  });

  toggleTheme();
}

function toggleTheme() {
  const toggleSwitch = document.querySelector('#checkbox');
  const currentTheme = localStorage.getItem('theme');

  // 1. Check: Gibt es eine gespeicherte Einstellung?
  if (currentTheme) {
      document.documentElement.style.colorScheme = currentTheme;
      if (currentTheme === 'dark') {
          toggleSwitch.checked = true;
      }
  } else {
      // 2. Fallback: System-Einstellung prüfen
      if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
          toggleSwitch.checked = true;
          // Wir setzen es nicht hart, damit der User noch wechseln kann,
          // aber der Switch muss visuell stimmen.
      }
  }

  toggleSwitch.addEventListener('change', switchTheme, false);
}
  // 3. Event Listener für den Switch
  function switchTheme(e) {
      if (e.target.checked) {
          document.documentElement.style.colorScheme = 'dark';
          localStorage.setItem('theme', 'dark');
      } else {
          document.documentElement.style.colorScheme = 'light';
          localStorage.setItem('theme', 'light');
      }
  }

