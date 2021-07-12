FROM alpine

ENV VERSION=3.1.4

RUN apk update && apk upgrade
RUN apk add gcc g++ bash
RUN apk add curl make tar
# RUN curl http://download.joedog.org/siege/siege-$VERSION.tar.gz > siege-$VERSION.tar.gz
# RUN tar -xf siege-${VERSION}.tar.gz
# RUN cd siege-${VERSION}
# RUN ./configure
# RUN make install

RUN mkdir -p /test

COPY * /test/

CMD sleep infinity