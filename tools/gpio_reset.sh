#!/bin/sh
set -eu

echo "=== GPIO GLOBAL RESET (libgpiod) ==="

if [ "$(id -u)" -ne 0 ]; then
  echo "Usa sudo: sudo $0"
  exit 1
fi

echo "[1/3] Termino processi che usano i gpiochip..."

for chip in /dev/gpiochip*; do
  [ -e "$chip" ] || continue
  if command -v fuser >/dev/null 2>&1; then
    PIDS="$(fuser "$chip" 2>/dev/null || true)"
    if [ -n "$PIDS" ]; then
      echo "  Kill su $chip: $PIDS"
      kill -9 $PIDS 2>/dev/null || true
    fi
  fi
done

echo "[2/3] Reset di tutte le linee a input..."

for chip in gpiochip0 gpiochip1 gpiochip2; do
  if gpioinfo $chip >/dev/null 2>&1; then
    MAX=$(gpioinfo $chip | grep "line" | wc -l)
    i=0
    while [ $i -lt $MAX ]; do
      # chiediamo la linea come input e rilasciamo subito
      gpioset --mode=exit $chip $i=0 2>/dev/null || true
      i=$((i+1))
    done
  fi
done

echo "[3/3] Stato attuale:"
gpioinfo gpiochip0 | head -n 20

echo "RESET COMPLETATO"