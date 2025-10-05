namespace VisitorBuilder;
using static Console;

public abstract class Expression;

public class DoubleExpression(double value) : Expression
{
  public readonly double Value = value;
}

public class AdditionExpression(Expression left, Expression right) : Expression
{
  public readonly Expression Left = left, Right = right;
}

public class MultiplicationExpression(Expression left, Expression right)
  : AdditionExpression(left, right);

public interface IVisitor<T, TResult>
{
  TResult Visit(IVisitor<T, TResult> visitor, T node);
  TResult Visit(T node);
}

public class VisitorBuilder<T, TResult>
{
  public static VisitorBuilder<T, TResult> New => new();
  private readonly Dictionary<Type, Func<IVisitor<T, TResult>, T, TResult>> visitors = new();
  private Func<IVisitor<T, TResult>, T, TResult> defaultVisitor;

  public VisitorBuilder<T, TResult> For<TNode>(Func<IVisitor<T, TResult>, TNode, TResult> visitor)
    where TNode : T
  {
    visitors[typeof(TNode)] = (v, node) => visitor(v, (TNode)node);
    return this;
  }

  public VisitorBuilder<T, TResult> Default(Func<IVisitor<T, TResult>, T, TResult> visitor)
  {
    defaultVisitor = visitor;
    return this;
  }
  
  public IVisitor<T, TResult> Build() => new BuiltVisitor(visitors, defaultVisitor);

  private class BuiltVisitor : IVisitor<T, TResult>
  {
    private readonly Dictionary<Type, Func<IVisitor<T, TResult>, T, TResult>> visitors;
    private readonly Func<IVisitor<T, TResult>, T, TResult> defaultVisitor;

    public BuiltVisitor(Dictionary<Type, Func<IVisitor<T, TResult>, T, TResult>> visitors, 
      Func<IVisitor<T, TResult>, T, TResult> defaultVisitor)
    {
      this.visitors = visitors;
      this.defaultVisitor = defaultVisitor;
    }

    public TResult Visit(IVisitor<T, TResult> self, T node)
    {
      var type = node.GetType();
      if (visitors.TryGetValue(type, out var visitor))
        return visitor(self, node);
      return defaultVisitor(self, node);
    }

    public TResult Visit(T node) => Visit(this, node);
  }
}

public class Demo
{
  public static void Main(string[] args)
  {
    var expression = new AdditionExpression(
      new DoubleExpression(5),
      new MultiplicationExpression(new DoubleExpression(3), new DoubleExpression(2))
    );

    var printer = VisitorBuilder<Expression, string>.New
      .For<DoubleExpression>((_, de) => de.Value.ToString())
      .For<AdditionExpression>((v, ae) => $"({v.Visit(v, ae.Left)}) + ({v.Visit(v, ae.Right)})")
      .For<MultiplicationExpression>((v, me) => $"({v.Visit(v, me.Left)}) * ({v.Visit(v, me.Right)})")
      .Build();
    
    var evaluator = VisitorBuilder<Expression, double>.New
      .For<DoubleExpression>((_, de) => de.Value)
      .For<AdditionExpression>((v, ae) => v.Visit(v, ae.Left) + v.Visit(v, ae.Right))
      .For<MultiplicationExpression>((v, me) => v.Visit(v, me.Left) * v.Visit(v, me.Right))
      .Build();

    var value = evaluator.Visit(expression);
    var print = printer.Visit(expression);
    WriteLine($"{print} = {value}"); // (5) + ((3) * (2)) = 11
  }
}
