if brew ls --versions telnet > /dev/null; then
  echo "Detected telnet, skipping installation\n"
  else
    echo "Installing telnet via brew..."
    brew install telnet
fi
echo "Building Dockerfile for CLion..."
docker build -t clion/debian/uni-bs -f Dockerfile .