self.addEventListener('install', (event) => {
  console.log("Service Worker Installed");
  // Ensure the SW becomes active immediately after install
  self.skipWaiting();
});

self.addEventListener('activate', (event) => {
  // Take control of uncontrolled clients
  event.waitUntil(self.clients.claim());
});

// Keep a basic fetch handler so SW file is actually usable.
self.addEventListener('fetch', (event) => {
  // Always go to network for core assets so PWA updates UI (style.css etc.)
  const url = new URL(event.request.url);

  // Only handle same-origin requests
  if (url.origin !== location.origin) return;

  if (event.request.method !== 'GET') return;

  event.respondWith(
    fetch(event.request)
      .then((res) => res)
      .catch(() => fetch(event.request))
  );
});


