#!/bin/bash

socat TCP4-LISTEN:54323,reuseaddr - | tee /tmp/tee.log
