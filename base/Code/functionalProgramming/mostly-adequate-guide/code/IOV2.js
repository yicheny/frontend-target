var IO = function(f) {
    this.unsafePerformIO = f;
}
  
IO.prototype.map = function(f) {
    return new IO(compose(f, this.unsafePerformIO));
}