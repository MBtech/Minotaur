#!/bin/bash
ps aux | grep "./app spout" | awk '{print $2}' | xargs -I {} kill -9 {}
ps aux | grep "./app splitter" | awk '{print $2}' | xargs -I {} kill -9 {}
ps aux | grep "./app count" | awk '{print $2}' | xargs -I {} kill -9 {}
