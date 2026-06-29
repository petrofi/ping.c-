# 🌐 C ile Basit Ping Aracı (ICMP Ping)

![C](https://img.shields.io/badge/Language-C-blue)
![Network](https://img.shields.io/badge/Protocol-ICMP-green)

C programlama dili ile yazılmış basit bir ICMP Echo (Ping) uygulaması. Linux/Unix sistemlerinde raw socket'ler kullanarak belirtilen bir IP adresine paket gönderir ve gecikmeyi (ping süresini) hesaplar.

## ⚙️ Gereksinimler
- GCC Derleyici
- Unix/Linux tabanlı bir işletim sistemi (Windows'ta çalışmaz, WSL veya Linux sanal makinesi gerekir)
- **Root/Sudo yetkisi** (Raw socket oluşturmak yetki gerektirir)

## 🚀 Derleme ve Çalıştırma

Projeyi derlemek için:
```bash
make
```

Çalıştırmak için:
```bash
sudo ./ping 8.8.8.8
```

Temizlemek için:
```bash
make clean
```
