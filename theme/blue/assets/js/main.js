window.onload = start;

function start() {
  let brand = document.querySelector(".header_left");
  brand.addEventListener("click", () => {
    window.location.href = "https://zheng-bote.github.io/cake-planner-backend/";
  });

  toggleTheme();
  shareBtn();

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


function shareBtn() {
  const shareBtn = document.getElementById('share-btn');
  if (!shareBtn) return;

  // Button IMMER anzeigen (da wir jetzt einen Fallback haben)
  shareBtn.style.display = 'inline-flex';

  shareBtn.addEventListener('click', async () => {
    // 1. Versuch: Native Web Share API (Mobile / unterstützte Browser)
    if (navigator.share) {
      try {
        await navigator.share({
          title: document.title,
          text: document.querySelector('meta[name="description"]')?.content || '',
          url: window.location.href,
        });
        return; // Wenn erfolgreich, hier aufhören
      } catch (err) {
        // Abbruch durch User ist okay, Fehler loggen
        if (err.name !== 'AbortError') console.debug('Share failed:', err);
      }
    }

    // 2. Fallback: URL in die Zwischenablage kopieren (Desktop)
    console.info('URL copied to clipboard');
    try {
      await navigator.clipboard.writeText(window.location.href);

      // Visuelles Feedback: Icon kurz ändern
      const originalIcon = shareBtn.innerHTML;

      // Checkmark Icon (passend zum Feather-Stil)
      shareBtn.innerHTML = `
        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="color: var(--primary-color);">
          <polyline points="20 6 9 17 4 12"></polyline>
        </svg>
      `;

      // Nach 2 Sekunden zurücksetzen
      setTimeout(() => {
        shareBtn.innerHTML = originalIcon;
      }, 2000);
    } catch (err) {
      console.error('Clipboard failed:', err);
      alert('Could not copy URL. Please copy it manually.');
    }
  });
}
