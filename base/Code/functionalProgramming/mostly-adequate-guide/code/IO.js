var IO = function(f) {
    this.__value = f;
}
  
IO.of = function(x) {
    return new IO(function() {
      return x;
    });
}
  
IO.prototype.map = function(f) {
    return new IO(compose(f, this.__value));
}

