FROM gcc:12.2

# Environment
ENV cmake="/cmake-3.26.2-linux-x86_64/bin/cmake"
ENV ctest="/cmake-3.26.2-linux-x86_64/bin/ctest"
ENV build_dir="3ccodelib-build-release"

# Install dependencies
RUN apt update; \
    apt install -qq -y libpng-dev libx11-dev xserver-xorg-dev xorg-dev

# Install CMake 3.26.2
RUN wget -nv https://github.com/Kitware/CMake/releases/download/v3.26.2/cmake-3.26.2-linux-x86_64.tar.gz \
      && tar xf cmake-3.26.2-linux-x86_64.tar.gz

# Setup workdir
COPY ./ /project/
WORKDIR /project

# Default command: Build, Test
CMD ${cmake} -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ENV_GUI=False && \
    ${cmake} --build "${build_dir}" --target all -j 3 && \
    ${ctest} --verbose -C Release --test-dir "${build_dir}" && sh
