# CronuSwim [![Build Status](https://travis-ci.org/antnolang/CronuSwim.svg?branch=master)](https://travis-ci.org/antnolang/CronuSwim) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=antnolang_CronuSwim&metric=alert_status)](https://sonarcloud.io/dashboard?id=antnolang_CronuSwim)

Cronuswim is an embedded software designed to help swimming enthusiasts to measure their times in the pool very accurately without the help of a coach.

It works with [Arduino](https://www.arduino.cc/) and its main advantage is that the timer starts and stops automatically thanks to an implementation of [Naive Bayes algorithm](https://en.wikipedia.org/wiki/Naive_Bayes_classifier). This software, through various motion sensors located on the arduino board, is capable of detecting when the swimmer starts and finishes.

In this way, the swimmer saves the time it would take to start the stopwatch just before leaving and stop it just after finishing.

Therefore, we achieve more precise measurements and that the swimmer can fully focus on his swim instead of being aware of starting and stopping the timer as quickly as possible.

## Versioning

The versioning policy is inspired by [SemVer](https://semver.org/). Every version is made up of three numbers that follows the structure MAJOR.MINOR.PATCH:
- MAJOR: This number change from 0 to 1 when the software change from development stage to an stable production stage. It also increments by one when the compatible hardware of this software changes or when the machine-learning technique changes.
- MINOR: This number increments by one when a new feature is added without changing the compatible hardware.
- PATCH: This number increments by one when fixing some bugs or other stuffs that do not change the functionality of the software (tests, documentation, configurations of the CI, etc.)

See the [tags](https://github.com/antnolang/CronuSwim/tags) to see all versions of the software.

