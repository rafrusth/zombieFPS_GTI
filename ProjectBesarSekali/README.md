# 🧟 ZombieVerse — Blocky Zombie FPS

> Tugas Besar Grafik dan Teknik Interaktif — Kelompok 6  
> Universitas Diponegoro

---

## 📋 Deskripsi

Game first-person shooter berbasis OpenGL/GLUT dengan zombie yang bergerak menggunakan algoritma **A\* Pathfinding**. Player dapat bergerak bebas di arena, menembak zombie, dan bertahan hidup selama mungkin.

---

## 🗂️ Struktur Proyek

```
GamesZombieModular/
├── main.cpp          # Entry point, loop utama, input handler
├── globals.h         # Deklarasi semua variabel global & struct
├── globals.cpp       # Definisi variabel global
├── render.h          # Deklarasi fungsi render
├── render.cpp        # Implementasi render (zombie, meja, pohon, HUD, tekstur)
├── aabb.h            # Deklarasi AABB collision
├── aabb.cpp          # Implementasi AABB collision
├── pathfinding.h     # Deklarasi A* pathfinding
├── pathfinding.cpp   # Implementasi A* pathfinding & steering zombie
├── hud.h             # Deklarasi HUD
├── hud.cpp           # Implementasi HUD (health bar, minimap, game over)
├── game.h            # Deklarasi logika game
├── game.cpp          # Implementasi logika game (init, reset, zombie attack)
├── lantai.bmp        # Tekstur lantai
└── meja.bmp          # Tekstur meja
```

---

## ⚙️ Dependensi

| Library | Versi | Keterangan |
|---|---|---|
| OpenGL | - | Bawaan Windows (`opengl32`) |
| GLU | - | Bawaan Windows (`glu32`) |
| freeglut | 3.x | Window & input management |
| GCC/MinGW | 4.8.1+ | Compiler (Dev-C++ / MSYS2) |

---

## 🔧 Cara Build

### Dev-C++ 5.7.1

1. Buka Dev-C++ → **File → New → Project → Empty Project**
2. Tambahkan semua file `.cpp` via **Project → Add to Project**
3. Set tipe project: **Project → Project Options → General → Win32 Console**
4. Tambahkan linker flags: **Project → Project Options → Parameters → Linker**:
   ```
   -lmingw32 -lopengl32 -lglu32 -lfreeglut
   ```
5. Pastikan `freeglut.dll` ada di folder yang sama dengan `.exe`
6. Tekan **F9** untuk Build & Run

### MSYS2 / Command Line

```bash
g++ -o ZombieVerse.exe main.cpp globals.cpp render.cpp aabb.cpp \
    pathfinding.cpp hud.cpp game.cpp \
    -lopengl32 -lglu32 -lfreeglut -std=c++11
```

---

## 🎮 Kontrol

| Tombol | Aksi |
|---|---|
| `W` `A` `S` `D` | Gerak maju / kiri / mundur / kanan |
| `Mouse` | Arahkan pandangan |
| `LMB` (klik kiri) | Tembak |
| `1` `2` `3` `4` | Ganti mode kamera |
| `=` / `-` | Zoom in / out (mode 2–4) |
| `R` | Restart (saat Game Over) |
| `ESC` | Keluar |

---

## 🧩 Arsitektur Kode

### Sistem Koordinat
- World space: X horizontal, Y vertikal, Z depth
- Grid pathfinding: **201×201 sel**, ukuran tiap sel **0.5 unit**
- Batas world: X dan Z dari **-50.0** sampai **+50.0**

### Modul Utama

#### `pathfinding.cpp`
- `buildPathGrid()` — membangun grid obstacle dari posisi meja & pohon
- `findPathAStar()` — algoritma A\* dengan 4-directional movement
- `updateZombie()` — steering & collision avoidance per zombie
- `calculateZombiePath()` — hitung ulang path zombie ke player

#### `render.cpp`
- `drawZombie()` — render model zombie blocky
- `drawTable()` / `drawPohon()` — render objek arena
- `drawHandHUD()` — render tangan/senjata di FPS mode
- `drawPlayer()` — render model player (mode 3rd person)
- `loadBMPTexture()` — load tekstur `.bmp` manual tanpa library tambahan

#### `aabb.cpp`
- `checkAABB()` — deteksi tabrakan axis-aligned bounding box
- `getTableAABB()` / `getZombieAABB()` / `getPlayerAABB()` — getter AABB per objek

#### `hud.cpp`
- `drawHUD()` — health bar, overlay game over, minimap
- Minimap ukuran **160px**, range world **40 unit**

#### `game.cpp`
- `initGame()` — spawn meja (random 3–12), pohon (5 posisi tetap), zombie (5)
- `resetGame()` — reset semua state tanpa restart program
- `zombieAttack()` — cek overlap AABB player vs zombie, damage **10 HP** per 1 detik

---

## 📐 Konstanta Penting (`globals.h`)

```cpp
const int   GRID_W      = 201;      // Lebar grid pathfinding
const int   GRID_H      = 201;      // Tinggi grid pathfinding
const float CELL_SIZE   = 0.5f;     // Ukuran 1 sel grid (world unit)
const float WORLD_MIN_X = -50.0f;   // Batas kiri world
const float WORLD_MIN_Z = -50.0f;   // Batas atas world
const int   LIMIT_ZOMBIES = 20;     // Maksimum zombie
const int   LIMIT_TABLES  = 20;     // Maksimum meja
const int   LIMIT_POHON   = 20;     // Maksimum pohon
```

---

## ⚠️ Catatan

- Path tekstur di `game.cpp` hardcoded — sesuaikan dengan path lokal kamu:
  ```cpp
  textureFloor = loadBMPTexture("lantai.bmp");
  textureTable = loadBMPTexture("meja.bmp");
  ```
- `freeglut.dll` harus ada di folder yang sama dengan `.exe` agar program bisa jalan
- Dikompilasi dengan GCC 4.8.1 (C++03 compatible, tanpa flag `-std=c++11`)

---

## 👥 Anggota Kelompok 6

| Nama | NIM |
|---|---|
| Rafif Setya Imaduddin | 24060124130115 |
| Raffi Arditama | 24060124120020 |
| Felicia Evelina | 24060124120012 |
| Anggita Kirana Puspa | 24060124130064 |
