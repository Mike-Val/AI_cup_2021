clean:
	@echo "Cleaning cache..."
	@if [ -d cmake-build-release ]; then rm -rf cmake-build-release; fi
	@mkdir cmake-build-release

build: clean
	@echo "Building..."
	cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release ..

compile: build
	@echo "Compiling..."
	cmake --build ./cmake-build-release --target AI_cup_2021

seed = 1092841564

ch130:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/ch130.tsp ./results/tours/ch130.tsp

d198:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/d198.tsp ./results/tours/d198.tsp

eil76:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/eil76.tsp ./results/tours/eil76.tsp

fl1577:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/fl1577.tsp ./results/tours/fl1577.tsp

kroA100:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/kroA100.tsp ./results/tours/kroA100.tsp

lin318:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/lin318.tsp ./results/tours/lin318.tsp

pcb442:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/pcb442.tsp ./results/tours/pcb442.tsp

pr439:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/pr439.tsp ./results/tours/pr439.tsp

rat783:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/rat783.tsp ./results/tours/rat783.tsp

u1060:
	@./cmake-build-release/AI_cup_2021 $(seed) ./problems/u1060.tsp ./results/tours/u1060.tsp