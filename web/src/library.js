/* Remembers the game data files on this device, one set per game version,
 * so that a recording can be opened without picking them again. */

const DATABASE = "tibiarc";
const STORE = "data-files";

function request(operation) {
  return new Promise((resolve, reject) => {
    operation.onsuccess = () => resolve(operation.result);
    operation.onerror = () => reject(operation.error);
  });
}

async function open() {
  const opening = indexedDB.open(DATABASE, 1);

  opening.onupgradeneeded = () => opening.result.createObjectStore(STORE);

  return request(opening);
}

/* Storage is a convenience: when it's unavailable (private windows, quota)
 * the player simply asks for the files every time. */
export async function recall(version) {
  try {
    const database = await open();
    const store = database.transaction(STORE).objectStore(STORE);

    return (await request(store.get(version))) ?? null;
  } catch {
    return null;
  }
}

export async function remember(version, files) {
  try {
    const database = await open();
    const store = database.transaction(STORE, "readwrite").objectStore(STORE);

    await request(store.put(files, version));
  } catch {
    /* See above. */
  }
}
