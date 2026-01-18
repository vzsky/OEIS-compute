# [OEIS Exploration](https://oeis.org)

One can prove that there is no uninteresting number but can one show that there is no uninteresting sequence of integers? 

## workflows
each commit should trigger a github action generating baseline artifacts and comparing to see regression. 
If regression is found you (I) would need to manually inspect that by manually download the artifact files, put it 
in `/artifacts` and run `./scripts/compare_artifacts.sh` then to overwrite the artifacts as a new baseline, do `./scripts/use_artifacts.sh`.
If expecting benchmark to improve, do the same and commit new benchmarks.

Apart from that most things should be via `./run`. The goto commands are 
- `./run [directory]` to run that 
- `./run test [module]` to test that 
- `./run bench [module]` to benchmark that 
- `./run profile [directory]` to profile that 
