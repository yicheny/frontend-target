import React from "react";
import { isValidElementType } from "react-is";
import PropTypes from "prop-types";
import invariant from "tiny-invariant";
import warning from "tiny-warning";

import RouterContext from "./RouterContext.js";
import matchPath from "./matchPath.js";

function isEmptyChildren(children) {
    return React.Children.count(children) === 0;
}

function evalChildrenDev(children, props, path) {
    const value = children(props);

    warning(
        value !== undefined,
        "You returned `undefined` from the `children` function of " +
        `<Route${path ? ` path="${path}"` : ""}>, but you ` +
        "should have returned a React element or `null`"
    );

    return value || null;
}

/**
 * 用于匹配单个路径和渲染的公共 API。
 */
class Route extends React.Component {
    render() {
        return (
            <RouterContext.Consumer>
                {context => {
                    invariant(context, "You should not use <Route> outside a <Router>");

                    const location = this.props.location || context.location;

                    //有computedMatch，最优先使用computedMatch
                    //有path，次优先自己生成match
                    //无path，使用上下文的match【稍微需要注意的，这里未必是Router传递的上下文，因为Route也会创建RouterContext】
                    const match = this.props.computedMatch
                        ? this.props.computedMatch // <Switch> already computed the match for us
                        : this.props.path
                            ? matchPath(location.pathname, this.props)
                            : context.match;

                    //context提供了history、staticContext、location、match
                    const props = { ...context, location, match };

                    let { children, component, render } = this.props;

                    if (Array.isArray(children) && children.length === 0) {
                        children = null;
                    }

                    //创建一个RouterContext
                    //有props.children，最优先渲染children
                    //有props.component，次优先渲染component
                    //props.render优先度在以上两种情况之后
                    return (
                        <RouterContext.Provider value={props}>
                            {props.match
                                ? children
                                    ? typeof children === "function"
                                        ? __DEV__
                                            ? evalChildrenDev(children, props, this.props.path)
                                            : children(props)
                                        : children
                                    : component
                                        ? React.createElement(component, props)
                                        : render
                                            ? render(props)
                                            : null
                                : typeof children === "function"
                                    ? __DEV__
                                        ? evalChildrenDev(children, props, this.props.path)
                                        : children(props)
                                    : null}
                        </RouterContext.Provider>
                    );
                }}
            </RouterContext.Consumer>
        );
    }
}

if (__DEV__) {
    Route.propTypes = {
        children: PropTypes.oneOfType([PropTypes.func, PropTypes.node]),
        component: (props, propName) => {
            if (props[propName] && !isValidElementType(props[propName])) {
                return new Error(
                    `Invalid prop 'component' supplied to 'Route': the prop is not a valid React component`
                );
            }
        },
        exact: PropTypes.bool,
        location: PropTypes.object,
        path: PropTypes.oneOfType([
            PropTypes.string,
            PropTypes.arrayOf(PropTypes.string)
        ]),
        render: PropTypes.func,
        sensitive: PropTypes.bool,
        strict: PropTypes.bool
    };

    Route.prototype.componentDidMount = function () {
        warning(
            !(
                this.props.children &&
                !isEmptyChildren(this.props.children) &&
                this.props.component
            ),
            "You should not use <Route component> and <Route children> in the same route; <Route component> will be ignored"
        );

        warning(
            !(
                this.props.children &&
                !isEmptyChildren(this.props.children) &&
                this.props.render
            ),
            "You should not use <Route render> and <Route children> in the same route; <Route render> will be ignored"
        );

        warning(
            !(this.props.component && this.props.render),
            "You should not use <Route component> and <Route render> in the same route; <Route render> will be ignored"
        );
    };

    Route.prototype.componentDidUpdate = function (prevProps) {
        warning(
            !(this.props.location && !prevProps.location),
            '<Route> elements should not change from uncontrolled to controlled (or vice versa). You initially used no "location" prop and then provided one on a subsequent render.'
        );

        warning(
            !(!this.props.location && prevProps.location),
            '<Route> elements should not change from controlled to uncontrolled (or vice versa). You provided a "location" prop initially but omitted it on a subsequent render.'
        );
    };
}

export default Route;
