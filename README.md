# ZombieVerse 🧟‍♀️

<p style="text-align: justify;">
Game ini merupakan tembak-menembak untuk mengalahkan segerombolan zombie yang sedang mengincarmu.. Ambil ammo, berlindung di balik pohon maupun meja, tetapi mereka akan tetap mengejar.. 🗿
  
Siapkan bidikan senjata yang presisi untuk menembak semua zombie yang ada! Dengan begitu, para zombie akan meninggal. Jika beruntung menembakkan semua zombie.. anda akan menang! Jika tidak beruntung dan meninggal dimakan zombie.. anda akan kalah.
</p>

<img width="480" height="270" alt="ZombieVerse" src="https://github.com/user-attachments/assets/bea334f8-78c3-4959-afa5-c7aab7aeca5d" />

---

## Authors 👨‍💻👩‍💻

| Nama | Github |
|---|---|
| Setya | [@rafrusth](https://www.github.com/rafrusth) |
| Raffi | [@Zugz-Wan](https://www.github.com/Zugz-Wan) |
| Felis | [@Feliscantik](https://www.github.com/Feliscantik) |
| Anggi | [@anggitakirana](https://www.github.com/anggitakirana) |

## Dependensi ⚙️

| Library | Versi | Keterangan |
|---|---|---|
| OpenGL | - | Bawaan Windows (`opengl32`) |
| GLU | - | Bawaan Windows (`glu32`) |
| freeglut | 3.x | Window & Input Management |
| GCC/MinGW | 4.8.1+ | Compiler (Dev-C++ / MSYS2) |

## Cara Build 🔧

#### Dev-C++ 5.7.1

1. Buka Dev-C++ → **File → New → Project → Empty Project**
2. Tambahkan semua file `.cpp` via **Project → Add to Project**
3. Set tipe project: **Project → Project Options → General → Win32 Console**
4. Tambahkan linker flags: **Project → Project Options → Parameters → Linker**:
   ```
   -lmingw32 -lopengl32 -lglu32 -lfreeglut
   ```
5. Pastikan `freeglut.dll` ada di folder yang sama dengan `.exe`
6. Tekan **F9** untuk Build & Run

## Kontrol 🎮

| Tombol | Aksi |
|---|---|
| `W` `A` `S` `D` | Gerak Maju / Kiri / Mundur / Kanan |
| `Mouse` | Arahkan Pandangan |
| `LMB` (Klik Mouse Kiri) | Tembak |
| `1` `2` `3` `4` | Ganti Mode Kamera |
| `=` / `-` | Zoom In / Out (Mode 2-4) |
| `R` | Restart (saat Game Over atau Menang) |
| `ESC` | Keluar |

## Tech Stack 🛠️

- **Custom Game Engine:** C++
- **Library:** Glut

---

## Screenshots 📸
- Start Condition
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/dc794c34-c876-4e68-97d7-c9c2ecec2fa8" />

- Win Condition
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/2e4933f1-94aa-4cc7-a911-b7f7cf5334dd" />

- Lose Condition
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/9cf1ff4a-d884-49b4-9264-bb2bfa91181e" />





